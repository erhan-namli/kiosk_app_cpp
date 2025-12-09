#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QString>
#include <QFuture>
#include "AppSettings.h"

class ImageProcessor : public QObject {
    Q_OBJECT

public:
    explicit ImageProcessor(AppSettings* settings, QObject* parent = nullptr);
    ~ImageProcessor();

    void processImage(const QString& inputPath, const QString& outputPath);

signals:
    void processingStarted();
    void processingProgress(int percentage);
    void processingCompleted(const QString& outputPath);
    void processingFailed(const QString& errorMessage);

private:
    AppSettings* m_settings;
    QFuture<void> m_processingFuture;

    void processImageTask(const QString& inputPath, const QString& outputPath);
    bool cropAndConvert(const QString& inputPath, const QString& outputPath);
};

#endif // IMAGEPROCESSOR_H
