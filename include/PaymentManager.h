#ifndef PAYMENTMANAGER_H
#define PAYMENTMANAGER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "AppSettings.h"

class PaymentManager : public QObject {
    Q_OBJECT

public:
    explicit PaymentManager(AppSettings* settings, QObject* parent = nullptr);
    ~PaymentManager();

    bool createPaymentLink(int quantity, const QString& buyerPhone);
    void startPaymentPolling();
    void stopPaymentPolling();
    QString getCheckoutUrl() const;
    void reset();

signals:
    void paymentLinkCreated(const QString& qrBase64, const QString& checkoutUrl);
    void paymentVerified(const QString& orderId);
    void paymentFailed(const QString& errorMessage);
    void paymentTimeout();

private slots:
    void onPaymentLinkCreated(QNetworkReply* reply);
    void onPaymentStatusChecked(QNetworkReply* reply);
    void checkPaymentStatus();
    void onTimeout();

private:
    AppSettings* m_settings;
    QNetworkAccessManager* m_networkManager;
    QString m_paymentLinkId;
    QString m_checkoutUrl;
    QTimer* m_pollTimer;
    QTimer* m_timeoutTimer;

    QString generateQRCode(const QString& url);
    QNetworkRequest createSquareRequest(const QString& endpoint);
};

#endif // PAYMENTMANAGER_H
