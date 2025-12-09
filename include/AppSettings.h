#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QMap>
#include <QDir>

class AppSettings {
public:
    AppSettings();

    // Testing modes
    bool demoMode;        // Mock everything (scanner + payment)
    bool scannerOnlyMode; // Skip payment, use real scanner

    // Paths
    QDir appDir;
    QDir scansDir;
    QDir logsDir;

    // Scanner settings for Fujitsu fi-800R
    int scannerDpi;
    QString scannerMode;
    QString scannerFormat;
    QString scannerDevice; // Device name for fi-800R

    // Crop dimensions (will be adjusted for fi-800R if needed)
    int cropX1, cropY1, cropX2, cropY2;

    // Timeouts
    int scanTimeout;      // seconds
    int paymentTimeout;   // seconds
    int inactivityTimeout; // seconds (60s as per new design)

    // Email settings
    QString smtpServer;
    int smtpPort;
    QString emailUser;
    QString emailPassword;
    QString senderEmail;

    // Square API settings
    QString squareAccessToken;
    QString squareLocationId;
    QString squareApiBase;
    QString squareApiVersion;

    // Pricing (in cents) - updated to match new design
    // 1 credit = $3, 4 credits = $10
    QMap<int, int> prices;

    // Image processing
    int jpegQuality;
    int cropDetectionThreshold;

    // UI Settings
    int windowWidth;
    int windowHeight;
    bool fullscreen;

    // Helper methods
    QMap<QString, QString> getSquareHeaders() const;
    double getPriceDollars(int quantity) const;
    int getPriceCents(int quantity) const;

private:
    void createDirectories();
    void loadFromEnvironment();
};

#endif // APPSETTINGS_H
