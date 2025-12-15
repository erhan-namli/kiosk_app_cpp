#include "ImageProcessor.h"
#include <QImage>
#include <QDebug>
#include <QtConcurrent>
#include <jpeglib.h>
#include <tiffio.h>
#include <opencv2/opencv.hpp>

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
    // Validate input parameters
    if (inputPath.isEmpty()) {
        qCritical() << "ImageProcessor: Input path is empty";
        return false;
    }

    if (outputPath.isEmpty()) {
        qCritical() << "ImageProcessor: Output path is empty";
        return false;
    }

    if (!m_settings) {
        qCritical() << "ImageProcessor: Settings object is null";
        return false;
    }

    try {
        // Load image using OpenCV for auto-crop detection
        cv::Mat cvImage;
        try {
            cvImage = cv::imread(inputPath.toStdString(), cv::IMREAD_COLOR);
        } catch (const cv::Exception& e) {
            qCritical() << "ImageProcessor: OpenCV exception loading image:" << e.what();
            return false;
        } catch (const std::exception& e) {
            qCritical() << "ImageProcessor: Exception loading image:" << e.what();
            return false;
        }

        if (cvImage.empty()) {
            qCritical() << "ImageProcessor: Failed to load image:" << inputPath;
            return false;
        }

        qInfo() << "ImageProcessor: Loaded image:" << cvImage.cols << "x" << cvImage.rows;

        // Detect photo bounds using auto-crop
        qInfo() << "ImageProcessor: Detecting photo boundaries...";
        AutoCrop::CropResult result = m_autoCrop.detectPhotoBounds(cvImage, m_settings->cropDetectionThreshold);

        QRect cropRect;
        if (!result.success) {
            qWarning() << "ImageProcessor: Auto-crop detection failed:" << result.errorMessage;
            qInfo() << "ImageProcessor: Falling back to manual crop settings";

            // Fallback to manual crop settings
            int width = m_settings->cropX2 - m_settings->cropX1;
            int height = m_settings->cropY2 - m_settings->cropY1;

            if (width <= 0 || height <= 0) {
                qWarning() << "ImageProcessor: Invalid manual crop dimensions:" << width << "x" << height;
                // Use entire image as last resort
                cropRect = QRect(0, 0, cvImage.cols, cvImage.rows);
            } else {
                cropRect = QRect(m_settings->cropX1, m_settings->cropY1, width, height);
            }
        } else {
            cropRect = result.cropRect;
            qInfo() << "ImageProcessor: Auto-crop successful. Bounds:" << cropRect;
        }

        // Validate crop rectangle
        if (cropRect.width() <= 0 || cropRect.height() <= 0) {
            qCritical() << "ImageProcessor: Invalid crop rectangle dimensions:"
                       << cropRect.width() << "x" << cropRect.height();
            return false;
        }

        // Crop the image using OpenCV
        cv::Rect cvCropRect(cropRect.x(), cropRect.y(), cropRect.width(), cropRect.height());

        // Ensure crop rect is within image bounds
        cvCropRect.x = std::max(0, cvCropRect.x);
        cvCropRect.y = std::max(0, cvCropRect.y);
        cvCropRect.width = std::min(cvImage.cols - cvCropRect.x, cvCropRect.width);
        cvCropRect.height = std::min(cvImage.rows - cvCropRect.y, cvCropRect.height);

        // Final validation
        if (cvCropRect.width <= 0 || cvCropRect.height <= 0 ||
            cvCropRect.x < 0 || cvCropRect.y < 0 ||
            cvCropRect.x + cvCropRect.width > cvImage.cols ||
            cvCropRect.y + cvCropRect.height > cvImage.rows) {
            qCritical() << "ImageProcessor: Crop rectangle out of bounds:"
                       << "Rect(" << cvCropRect.x << "," << cvCropRect.y << ","
                       << cvCropRect.width << "," << cvCropRect.height << ")"
                       << "Image(" << cvImage.cols << "x" << cvImage.rows << ")";
            return false;
        }

        cv::Mat cropped;
        try {
            cropped = cvImage(cvCropRect);
        } catch (const cv::Exception& e) {
            qCritical() << "ImageProcessor: OpenCV exception during crop:" << e.what();
            return false;
        } catch (const std::exception& e) {
            qCritical() << "ImageProcessor: Exception during crop:" << e.what();
            return false;
        }

        if (cropped.empty()) {
            qCritical() << "ImageProcessor: Cropped image is empty";
            return false;
        }

        qInfo() << "ImageProcessor: Cropped to:" << cropped.cols << "x" << cropped.rows;

        // Validate JPEG quality
        int quality = m_settings->jpegQuality;
        if (quality < 0 || quality > 100) {
            qWarning() << "ImageProcessor: Invalid JPEG quality" << quality << "- using 85";
            quality = 85;
        }

        // Save as JPEG with specified quality using OpenCV
        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(quality);

        bool success = false;
        try {
            success = cv::imwrite(outputPath.toStdString(), cropped, compression_params);
        } catch (const cv::Exception& e) {
            qCritical() << "ImageProcessor: OpenCV exception saving JPEG:" << e.what();
            return false;
        } catch (const std::exception& e) {
            qCritical() << "ImageProcessor: Exception saving JPEG:" << e.what();
            return false;
        }

        if (!success) {
            qCritical() << "ImageProcessor: Failed to save JPEG:" << outputPath;
            return false;
        }

        qInfo() << "ImageProcessor: Image auto-cropped and converted successfully to:" << outputPath;
        return true;

    } catch (const cv::Exception& e) {
        qCritical() << "ImageProcessor: OpenCV exception in cropAndConvert:" << e.what();
        return false;
    } catch (const std::exception& e) {
        qCritical() << "ImageProcessor: Standard exception in cropAndConvert:" << e.what();
        return false;
    } catch (...) {
        qCritical() << "ImageProcessor: Unknown exception in cropAndConvert";
        return false;
    }
}
