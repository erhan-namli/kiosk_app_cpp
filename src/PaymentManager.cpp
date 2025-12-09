#include "PaymentManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QBuffer>
#include <QDebug>
#include <QProcess>

PaymentManager::PaymentManager(AppSettings* settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_paymentLinkId("")
    , m_checkoutUrl("")
{
    // Setup polling timer (2 seconds interval)
    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(2000);
    connect(m_pollTimer, &QTimer::timeout, this, &PaymentManager::checkPaymentStatus);

    // Setup timeout timer
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &PaymentManager::onTimeout);
}

PaymentManager::~PaymentManager() {
    stopPaymentPolling();
}

bool PaymentManager::createPaymentLink(int quantity, const QString& buyerPhone) {
    // Demo mode: auto-approve after 3 seconds
    if (m_settings->demoMode) {
        qInfo() << "DEMO MODE: Mock payment for" << quantity << "credits";

        m_checkoutUrl = QString("https://square.link/demo/%1credits").arg(quantity);
        QString qrBase64 = generateQRCode(m_checkoutUrl);
        emit paymentLinkCreated(qrBase64, m_checkoutUrl);

        // Auto-approve after 3 seconds
        QTimer::singleShot(3000, this, [this]() {
            emit paymentVerified("demo-order-123");
        });

        return true;
    }

    // Scanner-only mode: skip payment processing
    if (m_settings->scannerOnlyMode) {
        qInfo() << "SCANNER ONLY MODE: Skipping payment for" << quantity << "credits";

        // Auto-approve immediately
        QTimer::singleShot(100, this, [this]() {
            emit paymentVerified("scanner-test-order");
        });

        return true;
    }

    qInfo() << "Creating payment link for" << quantity << "credits";

    // Get price
    int priceCents = m_settings->getPriceCents(quantity);
    if (priceCents == 0) {
        emit paymentFailed("Invalid quantity");
        return false;
    }

    // Prepare API request
    QString itemName = QString("Digitize %1 Photo %2")
                          .arg(quantity)
                          .arg(quantity > 1 ? "Strips" : "Strip");

    QJsonObject payload;
    payload["idempotency_key"] = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QJsonObject quickPay;
    quickPay["name"] = itemName;

    QJsonObject priceMoney;
    priceMoney["amount"] = priceCents;
    priceMoney["currency"] = "USD";
    quickPay["price_money"] = priceMoney;
    quickPay["location_id"] = m_settings->squareLocationId;

    payload["quick_pay"] = quickPay;

    QJsonObject checkoutOptions;
    checkoutOptions["allow_tipping"] = false;
    payload["checkout_options"] = checkoutOptions;

    // Note: Phone number is stored for SMS delivery, not sent to Square
    // Square payment links use email. You may want to collect email instead
    // or use a default email format like phone@kiosk.local
    QJsonObject prePopulatedData;
    prePopulatedData["buyer_email"] = QString("%1@kiosk.local").arg(buyerPhone);
    payload["pre_populated_data"] = prePopulatedData;

    // Make API request
    QNetworkRequest request = createSquareRequest("/v2/online-checkout/payment-links");
    QNetworkReply* reply = m_networkManager->post(request, QJsonDocument(payload).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onPaymentLinkCreated(reply);
    });

    return true;
}

void PaymentManager::onPaymentLinkCreated(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = reply->errorString();
        qCritical() << "Payment link creation failed:" << errorMsg;
        qCritical() << "Response:" << reply->readAll();
        emit paymentFailed("Payment system error");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject response = doc.object();

    if (!response.contains("payment_link")) {
        qCritical() << "Invalid response from Square API";
        emit paymentFailed("Payment system error");
        return;
    }

    QJsonObject paymentLink = response["payment_link"].toObject();
    m_paymentLinkId = paymentLink["id"].toString();

    // Get checkout URL (prefer long_url)
    if (paymentLink.contains("long_url")) {
        m_checkoutUrl = paymentLink["long_url"].toString();
    } else {
        m_checkoutUrl = paymentLink["url"].toString();
    }

    qInfo() << "Payment link created:" << m_paymentLinkId;

    // Generate QR code
    QString qrBase64 = generateQRCode(m_checkoutUrl);

    emit paymentLinkCreated(qrBase64, m_checkoutUrl);

    // Start polling for payment
    startPaymentPolling();
}

QString PaymentManager::generateQRCode(const QString& url) {
    // Use qrencode command-line tool to generate QR code
    QProcess process;
    QStringList args;
    args << "-s" << "10"; // Size
    args << "-l" << "L";  // Error correction level
    args << "-o" << "-";  // Output to stdout
    args << url;

    process.start("qrencode", args);
    process.waitForFinished();

    if (process.exitCode() == 0) {
        QByteArray pngData = process.readAllStandardOutput();
        return QString("data:image/png;base64,") + pngData.toBase64();
    }

    qWarning() << "QR code generation failed, using placeholder";
    return ""; // Return empty if qrencode fails
}

void PaymentManager::startPaymentPolling() {
    if (m_paymentLinkId.isEmpty()) {
        qWarning() << "Cannot start polling: no payment link ID";
        return;
    }

    qInfo() << "Starting payment polling";
    m_pollTimer->start();
    m_timeoutTimer->start(m_settings->paymentTimeout * 1000);
}

void PaymentManager::stopPaymentPolling() {
    qInfo() << "Stopping payment polling";
    m_pollTimer->stop();
    m_timeoutTimer->stop();
}

void PaymentManager::checkPaymentStatus() {
    if (m_paymentLinkId.isEmpty()) {
        return;
    }

    QString endpoint = QString("/v2/online-checkout/payment-links/%1").arg(m_paymentLinkId);
    QNetworkRequest request = createSquareRequest(endpoint);
    QNetworkReply* reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onPaymentStatusChecked(reply);
    });
}

void PaymentManager::onPaymentStatusChecked(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Payment status check failed:" << reply->errorString();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject response = doc.object();

    if (response.contains("payment_link")) {
        QJsonObject paymentLink = response["payment_link"].toObject();

        if (paymentLink.contains("order_id")) {
            QString orderId = paymentLink["order_id"].toString();
            qInfo() << "Payment verified:" << orderId;
            stopPaymentPolling();
            emit paymentVerified(orderId);
        }
    }
}

void PaymentManager::onTimeout() {
    qWarning() << "Payment timeout";
    stopPaymentPolling();
    emit paymentTimeout();
}

QString PaymentManager::getCheckoutUrl() const {
    return m_checkoutUrl;
}

void PaymentManager::reset() {
    stopPaymentPolling();
    m_paymentLinkId.clear();
    m_checkoutUrl.clear();
}

QNetworkRequest PaymentManager::createSquareRequest(const QString& endpoint) {
    QUrl url(m_settings->squareApiBase + endpoint);
    QNetworkRequest request(url);

    QMap<QString, QString> headers = m_settings->getSquareHeaders();
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    return request;
}
