#include "AppSettings.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QProcessEnvironment>

AppSettings::AppSettings()
    : demoMode(false)
    , scannerOnlyMode(false)
    , scannerDpi(600)
    , scannerMode("Color")
    , scannerFormat("tiff")
    , scannerDevice("") // Will be auto-detected
    , cropX1(0), cropY1(0), cropX2(1725), cropY2(1988)
    , scanTimeout(180)
    , paymentTimeout(300)
    , inactivityTimeout(60) // 60 seconds as per new design
    , smtpPort(587)
    , squareApiVersion("2024-11-13")
    , jpegQuality(92)
    , cropDetectionThreshold(240)
    , windowWidth(1024)
    , windowHeight(768)
    , fullscreen(false)
{
    // Setup paths
    appDir = QDir(QCoreApplication::applicationDirPath());
    scansDir = QDir(appDir.filePath("scans"));
    logsDir = QDir(appDir.filePath("logs"));

    createDirectories();
    loadFromEnvironment();

    // Setup pricing - updated to match new design
    // 1 credit = $3.00, 4 credits = $10.00
    prices[1] = 300;   // $3.00
    prices[4] = 1000;  // $10.00
}

void AppSettings::createDirectories() {
    if (!scansDir.exists()) {
        scansDir.mkpath(".");
    }
    if (!logsDir.exists()) {
        logsDir.mkpath(".");
    }

    // Create failed deliveries subdirectory
    QDir failedDir(scansDir.filePath("failed_deliveries"));
    if (!failedDir.exists()) {
        failedDir.mkpath(".");
    }
}

void AppSettings::loadFromEnvironment() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // SMTP settings
    smtpServer = env.value("SMTP_SERVER", "smtp.gmail.com");
    smtpPort = env.value("SMTP_PORT", "587").toInt();
    emailUser = env.value("EMAIL_USER", "caseywatters11@gmail.com");
    emailPassword = env.value("EMAIL_PASSWORD", "rlpp jouh izng xjmw");
    senderEmail = env.value("SENDER_EMAIL", "caseywatters11@gmail.com");

    // Square API settings
    squareAccessToken = env.value("SQUARE_ACCESS_TOKEN",
        "EAAAl7Jvtdh62sIqKkV40Nuhog_hdl5cakKG1dxgd1fJ2IGMQJ0hyUIBwdHapWnK");
    squareLocationId = env.value("SQUARE_LOCATION_ID", "LNDWT6XQMEBYS");
    squareApiBase = env.value("SQUARE_API_BASE", "https://connect.squareupsandbox.com");

    // UI settings
    fullscreen = env.value("KIOSK_FULLSCREEN", "false").toLower() == "true";

    // Testing modes
    demoMode = env.value("DEMO_MODE", "false").toLower() == "true";
    scannerOnlyMode = env.value("SCANNER_ONLY", "false").toLower() == "true";
}

QMap<QString, QString> AppSettings::getSquareHeaders() const {
    QMap<QString, QString> headers;
    headers["Square-Version"] = squareApiVersion;
    headers["Authorization"] = QString("Bearer %1").arg(squareAccessToken);
    headers["Content-Type"] = "application/json";
    return headers;
}

double AppSettings::getPriceDollars(int quantity) const {
    return prices.value(quantity, 0) / 100.0;
}

int AppSettings::getPriceCents(int quantity) const {
    return prices.value(quantity, 0);
}
