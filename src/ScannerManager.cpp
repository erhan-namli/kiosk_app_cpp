#include "ScannerManager.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QImage>
#include <QPainter>

ScannerManager::ScannerManager(AppSettings* settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)
    , m_deviceName("")
    , m_scanProcess(nullptr)
{
}

ScannerManager::~ScannerManager() {
    if (m_scanProcess) {
        m_scanProcess->kill();
        m_scanProcess->waitForFinished();
        delete m_scanProcess;
    }
}

bool ScannerManager::detectScanner() {
    // Demo mode: always succeed
    if (m_settings->demoMode) {
        m_deviceName = "demo-scanner (mock)";
        qInfo() << "DEMO MODE: Mock scanner detected";
        emit scannerDetected(m_deviceName);
        return true;
    }

    qInfo() << "Detecting scanner...";

    QProcess process;
    process.start("scanimage", QStringList() << "-L");

    if (!process.waitForFinished(10000)) {
        qWarning() << "Scanner detection timeout";
        emit scannerNotFound();
        return false;
    }

    QString output = process.readAllStandardOutput() + process.readAllStandardError();

    // Check for no scanners
    if (output.contains("No scanners were identified")) {
        qWarning() << "No scanners detected";
        emit scannerNotFound();
        return false;
    }

    // Look for Fujitsu fi-800R (uses fujitsu or epsonds backend)
    // Example: "device `fujitsu:ScanSnap fi-800R:xxxxx' is a FUJITSU fi-800R scanner"
    QRegularExpression devicePattern("device `([^']+)'");
    QRegularExpressionMatchIterator it = devicePattern.globalMatch(output);

    // First pass: look for Fujitsu scanners specifically
    QStringList allDevices;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString device = match.captured(1);
        allDevices.append(device);

        // Prefer fujitsu backend for fi-800R
        if (device.contains("fujitsu", Qt::CaseInsensitive) ||
            device.contains("fi-800", Qt::CaseInsensitive)) {
            m_deviceName = device;
            m_settings->scannerDevice = device;
            qInfo() << "Scanner detected:" << m_deviceName;
            emit scannerDetected(m_deviceName);
            return true;
        }
    }

    // Fall back to first available scanner
    if (!allDevices.isEmpty()) {
        m_deviceName = allDevices.first();
        m_settings->scannerDevice = m_deviceName;
        qInfo() << "Generic scanner detected:" << m_deviceName;
        emit scannerDetected(m_deviceName);
        return true;
    }

    qWarning() << "No compatible scanner found";
    emit scannerNotFound();
    return false;
}

bool ScannerManager::performScan(const QString& outputPath) {
    if (m_deviceName.isEmpty()) {
        qCritical() << "Cannot scan: No scanner device set";
        emit scanFailed("No scanner detected");
        return false;
    }

    // Demo mode: create fake TIFF file
    if (m_settings->demoMode) {
        emit scanStarted();
        qInfo() << "DEMO MODE: Mock scanning to" << outputPath;
        createDemoScan(outputPath);
        emit scanCompleted(outputPath);
        return true;
    }

    emit scanStarted();
    qInfo() << "Starting scan to" << outputPath;

    // Build scanimage command for Fujitsu fi-800R
    QStringList args;
    args << "--device-name" << m_deviceName;
    args << "--source" << "Card Front";  // Use card feeder front side
    args << "--format" << m_settings->scannerFormat;
    args << "--resolution" << QString::number(m_settings->scannerDpi);
    args << "--mode" << m_settings->scannerMode;

    // Output to file
    args << "-o" << outputPath;

    m_scanProcess = new QProcess(this);
    m_scanProcess->start("scanimage", args);

    if (!m_scanProcess->waitForFinished(m_settings->scanTimeout * 1000)) {
        qCritical() << "Scan timeout";
        emit scanFailed("Scan timeout - please try again");
        delete m_scanProcess;
        m_scanProcess = nullptr;
        return false;
    }

    if (m_scanProcess->exitCode() != 0) {
        QString errorMsg = m_scanProcess->readAllStandardError();
        qCritical() << "Scan failed:" << errorMsg;
        emit scanFailed("Scanner error: " + errorMsg);
        delete m_scanProcess;
        m_scanProcess = nullptr;
        return false;
    }

    delete m_scanProcess;
    m_scanProcess = nullptr;

    // Verify file was created and has content
    QFile file(outputPath);
    if (!file.exists() || file.size() == 0) {
        qCritical() << "Scan produced no output";
        emit scanFailed("Scanner produced empty file");
        return false;
    }

    qInfo() << "Scan completed successfully";
    emit scanCompleted(outputPath);
    return true;
}

bool ScannerManager::isAvailable() const {
    return !m_deviceName.isEmpty();
}

QString ScannerManager::getDeviceName() const {
    return m_deviceName.isEmpty() ? "Not detected" : m_deviceName;
}

QStringList ScannerManager::listAvailableScanners() {
    QStringList scanners;

    QProcess process;
    process.start("scanimage", QStringList() << "-L");

    if (!process.waitForFinished(10000)) {
        return scanners;
    }

    QString output = process.readAllStandardOutput();
    QRegularExpression devicePattern("device `([^']+)'");
    QRegularExpressionMatchIterator it = devicePattern.globalMatch(output);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        scanners.append(match.captured(1));
    }

    return scanners;
}

void ScannerManager::createDemoScan(const QString& outputPath) {
    // Create a fake scan (white image with colored rectangles to simulate photo strip)
    QImage image(1725, 1988, QImage::Format_RGB888);
    image.fill(Qt::white);

    QPainter painter(&image);

    // Add colored boxes to simulate photo strip
    painter.fillRect(300, 200, 1125, 400, QColor(255, 200, 200)); // Pink
    painter.fillRect(300, 700, 1125, 400, QColor(200, 255, 200)); // Green
    painter.fillRect(300, 1200, 1125, 400, QColor(200, 200, 255)); // Blue

    painter.end();

    // Save as TIFF
    image.save(outputPath, "TIFF");
    qInfo() << "DEMO MODE: Mock scan completed";
}
