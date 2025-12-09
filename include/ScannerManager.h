#ifndef SCANNERMANAGER_H
#define SCANNERMANAGER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include "AppSettings.h"

class ScannerManager : public QObject {
    Q_OBJECT

public:
    explicit ScannerManager(AppSettings* settings, QObject* parent = nullptr);
    ~ScannerManager();

    bool detectScanner();
    bool performScan(const QString& outputPath);
    bool isAvailable() const;
    QString getDeviceName() const;
    QStringList listAvailableScanners();

signals:
    void scannerDetected(const QString& deviceName);
    void scannerNotFound();
    void scanStarted();
    void scanProgress(int percentage);
    void scanCompleted(const QString& filePath);
    void scanFailed(const QString& errorMessage);

private:
    AppSettings* m_settings;
    QString m_deviceName;
    QProcess* m_scanProcess;

    void createDemoScan(const QString& outputPath);
};

#endif // SCANNERMANAGER_H
