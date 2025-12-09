#include "ImageProcessor.h"
#include <QImage>
#include <QDebug>
#include <QtConcurrent>
#include <jpeglib.h>
#include <tiffio.h>

ImageProcessor::ImageProcessor(AppSettings* settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)
{
}

ImageProcessor::~ImageProcessor() {
    if (m_processingFuture.isRunning()) {
        m_processingFuture.waitForFinished();
    }
}

void ImageProcessor::processImage(const QString& inputPath, const QString& outputPath) {
    emit processingStarted();
    qInfo() << "Processing image:" << inputPath << "->" << outputPath;

    // Process in background thread
    m_processingFuture = QtConcurrent::run([this, inputPath, outputPath]() {
        processImageTask(inputPath, outputPath);
    });
}

void ImageProcessor::processImageTask(const QString& inputPath, const QString& outputPath) {
    try {
        bool success = cropAndConvert(inputPath, outputPath);

        if (success) {
            qInfo() << "Image processing completed:" << outputPath;
            emit processingCompleted(outputPath);
        } else {
            qCritical() << "Image processing failed";
            emit processingFailed("Failed to process image");
        }
    } catch (const std::exception& e) {
        qCritical() << "Image processing exception:" << e.what();
        emit processingFailed(QString("Processing error: %1").arg(e.what()));
    }
}

bool ImageProcessor::cropAndConvert(const QString& inputPath, const QString& outputPath) {
    // Load TIFF image using Qt
    QImage image(inputPath);

    if (image.isNull()) {
        qCritical() << "Failed to load image:" << inputPath;
        return false;
    }

    // Crop to specified region
    QRect cropRect(
        m_settings->cropX1,
        m_settings->cropY1,
        m_settings->cropX2 - m_settings->cropX1,
        m_settings->cropY2 - m_settings->cropY1
    );

    // Ensure crop rect is within image bounds
    cropRect = cropRect.intersected(image.rect());

    QImage cropped = image.copy(cropRect);

    if (cropped.isNull()) {
        qCritical() << "Failed to crop image";
        return false;
    }

    // Save as JPEG with specified quality
    bool success = cropped.save(outputPath, "JPEG", m_settings->jpegQuality);

    if (!success) {
        qCritical() << "Failed to save JPEG:" << outputPath;
        return false;
    }

    qInfo() << "Image cropped and converted successfully";
    return true;
}
