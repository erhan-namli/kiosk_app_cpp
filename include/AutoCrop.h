#ifndef AUTOCROP_H
#define AUTOCROP_H

#include <opencv2/opencv.hpp>
#include <QString>
#include <QRect>
#include <QDebug>

class AutoCrop {
public:
    struct CropResult {
        bool success;
        QRect cropRect;
        QString errorMessage;

        CropResult() : success(false) {}
        CropResult(const QRect& rect) : success(true), cropRect(rect) {}
        CropResult(const QString& error) : success(false), errorMessage(error) {}
    };

    AutoCrop();
    ~AutoCrop();

    // Main function to detect and crop the photo from scanned image
    CropResult detectPhotoBounds(const QString& imagePath, int threshold = 50);

    // Alternative: Detect from OpenCV Mat directly
    CropResult detectPhotoBounds(const cv::Mat& image, int threshold = 50);

    // Apply perspective correction if needed
    cv::Mat correctPerspective(const cv::Mat& image, const std::vector<cv::Point>& corners);

private:
    // Find the largest rectangular contour (the photo)
    std::vector<cv::Point> findLargestRectangle(const cv::Mat& image, int threshold);

    // Order points for perspective transform (top-left, top-right, bottom-right, bottom-left)
    std::vector<cv::Point> orderPoints(const std::vector<cv::Point>& points);

    // Convert contour to approximate rectangle
    bool approximateRectangle(const std::vector<cv::Point>& contour, std::vector<cv::Point>& corners);

    // Helper to calculate contour area
    double calculateArea(const std::vector<cv::Point>& contour);
};

#endif // AUTOCROP_H
