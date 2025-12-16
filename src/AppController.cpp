#include "AppController.h"
#include <QUuid>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

AppController::AppController(AppSettings* settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)
    , m_credits(0)
    , m_price(0)
    , m_currentScan(0)
    , m_isScanning(false)
{
    // Create managers
    m_scanner = new ScannerManager(settings, this);
    m_payment = new PaymentManager(settings, this);
    m_email = new EmailManager(settings, this);
    m_imageProcessor = new ImageProcessor(settings, this);

    connectSignals();
}

AppController::~AppController() {
}

void AppController::connectSignals() {
    // Scanner signals
    connect(m_scanner, &ScannerManager::scannerDetected,
            this, &AppController::onScannerDetected);
    connect(m_scanner, &ScannerManager::scannerNotFound,
            this, &AppController::onScannerNotFound);
    connect(m_scanner, &ScannerManager::scanCompleted,
            this, &AppController::onScanCompleted);
    connect(m_scanner, &ScannerManager::scanFailed,
            this, &AppController::onScanFailed);

    // Payment signals
    connect(m_payment, &PaymentManager::paymentLinkCreated,
            this, &AppController::onPaymentLinkCreated);
    connect(m_payment, &PaymentManager::paymentVerified,
            this, &AppController::onPaymentVerified);
    connect(m_payment, &PaymentManager::paymentFailed,
            this, &AppController::onPaymentFailed);
    connect(m_payment, &PaymentManager::paymentTimeout,
            this, &AppController::onPaymentTimeout);

    // Email signals
    connect(m_email, &EmailManager::emailSent,
            this, &AppController::onEmailSent);
    connect(m_email, &EmailManager::emailFailed,
            this, &AppController::onEmailFailed);

    // Image processor signals
    connect(m_imageProcessor, &ImageProcessor::processingCompleted,
            this, &AppController::onProcessingCompleted);
    connect(m_imageProcessor, &ImageProcessor::processingFailed,
            this, &AppController::onProcessingFailed);
}

void AppController::initialize() {
    qInfo() << "Initializing application...";

    // Detect scanner
    if (m_scanner->detectScanner()) {
        qInfo() << "Scanner ready!";
    } else {
        qWarning() << "Scanner not detected";
        if (!m_settings->demoMode) {
            qWarning() << "Please connect the Fujitsu fi-800R and restart, or run in demo mode";
        }
    }
}

void AppController::startNewSession() {
    qInfo() << "Starting new session";
    m_sessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_scanPaths.clear();
    m_currentScan = 0;
    emit currentScanChanged();
}

void AppController::setPhoneNumber(const QString& phone) {
    if (m_phoneNumber != phone) {
        m_phoneNumber = phone;
        emit phoneNumberChanged();
        qInfo() << "Phone number set:" << phone;
    }
}

void AppController::setPurchaseInfo(int credits, int price) {
    if (m_credits != credits) {
        m_credits = credits;
        emit creditsChanged();
    }
    if (m_price != price) {
        m_price = price;
        emit priceChanged();
    }
    qInfo() << "Purchase info set:" << credits << "credits for $" << price;

    // In real implementation, create payment link here
    if (!m_settings->demoMode) {
        m_payment->createPaymentLink(credits, m_phoneNumber);
    }
}

void AppController::startScanning() {
    qInfo() << "Starting scanning workflow";
    m_currentScan = 0;
    emit currentScanChanged();
}

void AppController::performNextScan() {
    if (m_currentScan >= m_credits) {
        qInfo() << "All scans completed, sending email";
        sendEmail();
        return;
    }

    m_currentScan++;
    emit currentScanChanged();
    qInfo() << "Performing scan" << m_currentScan << "of" << m_credits;
}

void AppController::executeScan() {
    qInfo() << "Executing scan" << (m_currentScan + 1);

    m_isScanning = true;
    emit isScanningChanged();

    // Create temporary scan file (TIFF format)
    QString scanFilename = QString("%1_scan_%2.tiff")
                              .arg(m_sessionId)
                              .arg(m_currentScan + 1);
    QString scanPath = m_settings->scansDir.filePath(scanFilename);

    // Perform scan
    if (!m_scanner->performScan(scanPath)) {
        qCritical() << "Scan failed";
        m_isScanning = false;
        emit isScanningChanged();
    }
}

void AppController::onScannerDetected(const QString& deviceName) {
    qInfo() << "Scanner detected:" << deviceName;
}

void AppController::onScannerNotFound() {
    qWarning() << "Scanner not found";
}

void AppController::onScanCompleted(const QString& filePath) {
    qInfo() << "Scan completed:" << filePath;

    // Process image (crop and convert to JPEG)
    QString outputFilename = QString("%1_strip_%2.jpg")
                                .arg(m_sessionId)
                                .arg(m_currentScan + 1);
    QString outputPath = m_settings->scansDir.filePath(outputFilename);

    m_imageProcessor->processImage(filePath, outputPath);
}

void AppController::onScanFailed(const QString& errorMessage) {
    qCritical() << "Scan failed:" << errorMessage;
    m_isScanning = false;
    emit isScanningChanged();
}

void AppController::onProcessingCompleted(const QString& outputPath) {
    qInfo() << "Processing completed:" << outputPath;
    m_scanPaths.append(outputPath);

    // Remove temporary TIFF file
    QString tiffPath = outputPath;
    tiffPath.replace(".jpg", ".tiff");
    QFile::remove(tiffPath);

    m_isScanning = false;
    emit isScanningChanged();

    m_currentScan++;
    emit currentScanChanged();

    if (m_currentScan < m_credits) {
        qInfo() << "Scan" << m_currentScan << "successful! Ready for next scan";
    } else {
        qInfo() << "All scans completed! Sending email...";
        sendEmail();
    }
}

void AppController::onProcessingFailed(const QString& errorMessage) {
    qCritical() << "Processing failed:" << errorMessage;
    m_isScanning = false;
    emit isScanningChanged();
}

void AppController::sendEmail() {
    qInfo() << "Sending" << m_scanPaths.count() << "scans to" << m_phoneNumber;

    // Convert phone number to email format for delivery
    // In production, you might use an SMS gateway or different delivery method
    QString email = QString("%1@sms.kiosk.local").arg(m_phoneNumber);

    if (!m_email->sendScans(email, m_scanPaths)) {
        // Email failed - save locally as fallback
        QString backupDir = m_settings->scansDir.filePath(
            QString("failed_deliveries/%1").arg(m_sessionId)
        );
        m_email->saveScansLocally(m_scanPaths, backupDir);
    }
}

void AppController::onEmailSent() {
    qInfo() << "Email sent successfully";

    // Clean up scan files
    cleanupScans();

    // Emit completion signal for QML
    emit scanningCompleted();
}

void AppController::onEmailFailed(const QString& errorMessage) {
    qCritical() << "Email failed:" << errorMessage;

    // Still emit completion to show confirmation screen
    emit scanningCompleted();
}

void AppController::cleanupScans() {
    // Create archive directory for storing processed scans
    QString archiveDir = m_settings->scansDir.filePath("archive");
    QDir dir;
    if (!dir.exists(archiveDir)) {
        dir.mkpath(archiveDir);
    }

    // Copy scans to archive before cleanup
    for (const QString& scanPath : m_scanPaths) {
        QFileInfo fileInfo(scanPath);
        QString archivePath = archiveDir + "/" + fileInfo.fileName();

        // Copy to archive
        if (QFile::copy(scanPath, archivePath)) {
            qInfo() << "Archived scan to:" << archivePath;
        } else {
            qWarning() << "Failed to archive scan:" << scanPath;
        }

        // Remove temporary file
        QFile::remove(scanPath);
    }
    m_scanPaths.clear();
}

void AppController::onPaymentLinkCreated(const QString& qrBase64,
                                          const QString& checkoutUrl) {
    qInfo() << "Payment link created:" << checkoutUrl;
}

void AppController::onPaymentVerified(const QString& orderId) {
    qInfo() << "Payment verified:" << orderId;
    startScanning();
}

void AppController::onPaymentFailed(const QString& errorMessage) {
    qCritical() << "Payment failed:" << errorMessage;
}

void AppController::onPaymentTimeout() {
    qWarning() << "Payment timeout";
}

void AppController::cancelSession() {
    qInfo() << "Session cancelled";
    m_payment->reset();
    cleanupScans();
    resetToIdle();
}

void AppController::resetToIdle() {
    qInfo() << "Resetting to idle";

    if (m_phoneNumber != "") {
        m_phoneNumber = "";
        emit phoneNumberChanged();
    }
    if (m_credits != 0) {
        m_credits = 0;
        emit creditsChanged();
    }
    if (m_price != 0) {
        m_price = 0;
        emit priceChanged();
    }
    if (m_currentScan != 0) {
        m_currentScan = 0;
        emit currentScanChanged();
    }
    if (m_isScanning) {
        m_isScanning = false;
        emit isScanningChanged();
    }

    m_sessionId.clear();
}
