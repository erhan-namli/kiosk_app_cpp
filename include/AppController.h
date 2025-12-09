#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "ScannerManager.h"
#include "PaymentManager.h"
#include "EmailManager.h"
#include "ImageProcessor.h"
#include "AppSettings.h"

class AppController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString phoneNumber READ phoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(int credits READ credits NOTIFY creditsChanged)
    Q_PROPERTY(int price READ price NOTIFY priceChanged)
    Q_PROPERTY(int currentScan READ currentScan NOTIFY currentScanChanged)
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY isScanningChanged)

public:
    explicit AppController(AppSettings* settings, QObject* parent = nullptr);
    ~AppController();

    void initialize();

    // Property getters
    QString phoneNumber() const { return m_phoneNumber; }
    int credits() const { return m_credits; }
    int price() const { return m_price; }
    int currentScan() const { return m_currentScan; }
    bool isScanning() const { return m_isScanning; }

public slots:
    // Workflow control
    void startNewSession();
    void setPhoneNumber(const QString& phone);
    void setPurchaseInfo(int credits, int price);
    void startScanning();
    void performNextScan();
    void executeScan();
    void cancelSession();
    void resetToIdle();

private slots:
    // Scanner event handlers
    void onScannerDetected(const QString& deviceName);
    void onScannerNotFound();
    void onScanCompleted(const QString& filePath);
    void onScanFailed(const QString& errorMessage);

    // Payment event handlers
    void onPaymentLinkCreated(const QString& qrBase64, const QString& checkoutUrl);
    void onPaymentVerified(const QString& orderId);
    void onPaymentFailed(const QString& errorMessage);
    void onPaymentTimeout();

    // Image processing handlers
    void onProcessingCompleted(const QString& outputPath);
    void onProcessingFailed(const QString& errorMessage);

    // Email handlers
    void onEmailSent();
    void onEmailFailed(const QString& errorMessage);

signals:
    // Property change signals
    void phoneNumberChanged();
    void creditsChanged();
    void priceChanged();
    void currentScanChanged();
    void isScanningChanged();

    // Workflow signals
    void scanningCompleted();

private:
    void connectSignals();
    void sendEmail();
    void cleanupScans();

    AppSettings* m_settings;

    // Managers
    ScannerManager* m_scanner;
    PaymentManager* m_payment;
    EmailManager* m_email;
    ImageProcessor* m_imageProcessor;

    // Session state
    QString m_phoneNumber;
    int m_credits;
    int m_price;
    int m_currentScan;
    bool m_isScanning;
    QStringList m_scanPaths;
    QString m_sessionId;
};

#endif // APPCONTROLLER_H
