#include "AutoCrop.h"
#include <algorithm>
#include <cmath>

AutoCrop::AutoCrop() {
}

AutoCrop::~AutoCrop() {
}

AutoCrop::CropResult AutoCrop::detectPhotoBounds(const QString& imagePath, int threshold) {
    if (imagePath.isEmpty()) {
        qCritical() << "Auto-crop: Image path is empty";
        return CropResult("Image path is empty");
    }

    // Load image from file
    cv::Mat image;
    try {
        image = cv::imread(imagePath.toStdString(), cv::IMREAD_COLOR);
    } catch (const cv::Exception& e) {
        qCritical() << "Auto-crop: OpenCV exception while loading image:" << e.what();
        return CropResult(QString("OpenCV load error: %1").arg(e.what()));
    } catch (const std::exception& e) {
        qCritical() << "Auto-crop: Exception while loading image:" << e.what();
        return CropResult(QString("Load error: %1").arg(e.what()));
    }

    if (image.empty()) {
        qCritical() << "Auto-crop: Failed to load image:" << imagePath;
        return CropResult(QString("Failed to load image: %1").arg(imagePath));
    }

    return detectPhotoBounds(image, threshold);
}

AutoCrop::CropResult AutoCrop::detectPhotoBounds(const cv::Mat& image, int threshold) {
    if (image.empty()) {
        qCritical() << "Auto-crop: Input image is empty";
        return CropResult("Input image is empty");
    }

    // Validate threshold
    if (threshold <= 0 || threshold > 255) {
        qWarning() << "Auto-crop: Invalid threshold" << threshold << "- using default 50";
        threshold = 50;
    }

    try {
        // Find the largest rectangle in the image
        std::vector<cv::Point> corners = findLargestRectangle(image, threshold);

        if (corners.empty()) {
            qWarning() << "Auto-crop: No corners detected";
            // Fallback: return the entire image bounds
            return CropResult(QRect(0, 0, image.cols, image.rows));
        }

        if (corners.size() != 4) {
            qWarning() << "Auto-crop: Could not detect 4 corners, found:" << corners.size();
            // Fallback: return the entire image bounds
            return CropResult(QRect(0, 0, image.cols, image.rows));
        }

        // Order the corners
        std::vector<cv::Point> orderedCorners = orderPoints(corners);

        if (orderedCorners.size() != 4) {
            qWarning() << "Auto-crop: Point ordering failed";
            return CropResult(QRect(0, 0, image.cols, image.rows));
        }

        // Calculate bounding rectangle
        cv::Rect boundingRect = cv::boundingRect(orderedCorners);

        // Validate bounding rectangle
        if (boundingRect.width <= 0 || boundingRect.height <= 0) {
            qWarning() << "Auto-crop: Invalid bounding rectangle dimensions";
            return CropResult(QRect(0, 0, image.cols, image.rows));
        }

        // Check if detected area is too small (less than 10% of image area)
        double detectedArea = boundingRect.width * boundingRect.height;
        double imageArea = image.cols * image.rows;
        if (detectedArea < imageArea * 0.1) {
            qWarning() << "Auto-crop: Detected area too small ("
                      << (detectedArea / imageArea * 100) << "% of image)";
            return CropResult(QRect(0, 0, image.cols, image.rows));
        }

        // Add small margin (5 pixels) to ensure we don't cut off edges
        int margin = 5;
        boundingRect.x = std::max(0, boundingRect.x - margin);
        boundingRect.y = std::max(0, boundingRect.y - margin);
        boundingRect.width = std::min(image.cols - boundingRect.x, boundingRect.width + 2 * margin);
        boundingRect.height = std::min(image.rows - boundingRect.y, boundingRect.height + 2 * margin);

        // Convert to QRect
        QRect cropRect(boundingRect.x, boundingRect.y, boundingRect.width, boundingRect.height);

        qInfo() << "Auto-crop detected bounds:" << cropRect
                << "(" << (detectedArea / imageArea * 100) << "% of image)";
        return CropResult(cropRect);

    } catch (const cv::Exception& e) {
        qCritical() << "Auto-crop: OpenCV exception:" << e.what();
        return CropResult(QString("OpenCV error: %1").arg(e.what()));
    } catch (const std::exception& e) {
        qCritical() << "Auto-crop: Standard exception:" << e.what();
        return CropResult(QString("Error: %1").arg(e.what()));
    } catch (...) {
        qCritical() << "Auto-crop: Unknown exception";
        return CropResult("Unknown error during detection");
    }
}

std::vector<cv::Point> AutoCrop::findLargestRectangle(const cv::Mat& image, int threshold) {
    try {
        if (image.empty()) {
            qWarning() << "Auto-crop: findLargestRectangle - empty image";
            return std::vector<cv::Point>();
        }

        // Convert to grayscale
        cv::Mat gray;
        try {
            if (image.channels() == 3 || image.channels() == 4) {
                cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
            } else if (image.channels() == 1) {
                gray = image.clone();
            } else {
                qWarning() << "Auto-crop: Unsupported channel count:" << image.channels();
                return std::vector<cv::Point>();
            }
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed to convert to grayscale:" << e.what();
            return std::vector<cv::Point>();
        }

        // Apply Gaussian blur to reduce noise
        cv::Mat blurred;
        try {
            cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed to apply Gaussian blur:" << e.what();
            return std::vector<cv::Point>();
        }

        // Edge detection using Canny
        cv::Mat edges;
        try {
            cv::Canny(blurred, edges, threshold, threshold * 2);
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed Canny edge detection:" << e.what();
            return std::vector<cv::Point>();
        }

        // Dilate edges to close gaps
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        cv::Mat dilated;
        try {
            cv::dilate(edges, dilated, kernel);
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed to dilate edges:" << e.what();
            return std::vector<cv::Point>();
        }

        // Find contours
        std::vector<std::vector<cv::Point>> contours;
        try {
            cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed to find contours:" << e.what();
            return std::vector<cv::Point>();
        }

        if (contours.empty()) {
            qWarning() << "Auto-crop: No contours found";
            return std::vector<cv::Point>();
        }

        // Find the largest contour by area
        double maxArea = 0;
        int maxAreaIdx = -1;

        for (size_t i = 0; i < contours.size(); i++) {
            try {
                double area = cv::contourArea(contours[i]);
                if (area > maxArea) {
                    maxArea = area;
                    maxAreaIdx = i;
                }
            } catch (const cv::Exception& e) {
                qWarning() << "Auto-crop: Error calculating contour area at index" << i << ":" << e.what();
                continue;
            }
        }

        if (maxAreaIdx < 0) {
            qWarning() << "Auto-crop: No valid contour found";
            return std::vector<cv::Point>();
        }

        qInfo() << "Auto-crop: Largest contour area:" << maxArea << "pixels ("
                << contours.size() << "total contours)";

        // Approximate the contour to a polygon
        std::vector<cv::Point> corners;
        if (!approximateRectangle(contours[maxAreaIdx], corners)) {
            qWarning() << "Auto-crop: Could not approximate contour to rectangle - using bounding rect";
            // Return bounding rect of the largest contour
            try {
                cv::Rect boundingRect = cv::boundingRect(contours[maxAreaIdx]);
                corners.push_back(cv::Point(boundingRect.x, boundingRect.y));
                corners.push_back(cv::Point(boundingRect.x + boundingRect.width, boundingRect.y));
                corners.push_back(cv::Point(boundingRect.x + boundingRect.width, boundingRect.y + boundingRect.height));
                corners.push_back(cv::Point(boundingRect.x, boundingRect.y + boundingRect.height));
            } catch (const cv::Exception& e) {
                qWarning() << "Auto-crop: Failed to get bounding rect:" << e.what();
                return std::vector<cv::Point>();
            }
        }

        return corners;

    } catch (const cv::Exception& e) {
        qCritical() << "Auto-crop: OpenCV exception in findLargestRectangle:" << e.what();
        return std::vector<cv::Point>();
    } catch (const std::exception& e) {
        qCritical() << "Auto-crop: Standard exception in findLargestRectangle:" << e.what();
        return std::vector<cv::Point>();
    } catch (...) {
        qCritical() << "Auto-crop: Unknown exception in findLargestRectangle";
        return std::vector<cv::Point>();
    }
}

bool AutoCrop::approximateRectangle(const std::vector<cv::Point>& contour, std::vector<cv::Point>& corners) {
    if (contour.empty()) {
        qWarning() << "Auto-crop: approximateRectangle - empty contour";
        return false;
    }

    try {
        // Calculate perimeter
        double perimeter = cv::arcLength(contour, true);
        if (perimeter <= 0) {
            qWarning() << "Auto-crop: Invalid perimeter:" << perimeter;
            return false;
        }

        // Approximate the contour with polygon
        double epsilon = 0.02 * perimeter;
        std::vector<cv::Point> approx;

        try {
            cv::approxPolyDP(contour, approx, epsilon, true);
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed to approximate polygon:" << e.what();
            return false;
        }

        // If we have exactly 4 points, we found a quadrilateral
        if (approx.size() == 4) {
            corners = approx;
            return true;
        }

        // If we have more or less than 4 points, try different epsilon values
        for (double mult = 0.01; mult <= 0.05; mult += 0.005) {
            epsilon = mult * perimeter;
            try {
                cv::approxPolyDP(contour, approx, epsilon, true);
                if (approx.size() == 4) {
                    corners = approx;
                    return true;
                }
            } catch (const cv::Exception& e) {
                qWarning() << "Auto-crop: Failed polygon approximation at mult" << mult << ":" << e.what();
                continue;
            }
        }

        qWarning() << "Auto-crop: Could not find 4-point approximation (best:" << approx.size() << "points)";
        return false;

    } catch (const cv::Exception& e) {
        qCritical() << "Auto-crop: OpenCV exception in approximateRectangle:" << e.what();
        return false;
    } catch (const std::exception& e) {
        qCritical() << "Auto-crop: Standard exception in approximateRectangle:" << e.what();
        return false;
    }
}

std::vector<cv::Point> AutoCrop::orderPoints(const std::vector<cv::Point>& points) {
    if (points.size() != 4) {
        return points;
    }

    std::vector<cv::Point> ordered(4);

    // Sort by sum (x + y): top-left has smallest sum, bottom-right has largest sum
    std::vector<std::pair<int, cv::Point>> sums;
    for (const auto& pt : points) {
        sums.push_back({pt.x + pt.y, pt});
    }
    std::sort(sums.begin(), sums.end());

    cv::Point topLeft = sums[0].second;
    cv::Point bottomRight = sums[3].second;

    // Sort by difference (y - x): top-right has smallest diff, bottom-left has largest diff
    std::vector<std::pair<int, cv::Point>> diffs;
    for (const auto& pt : points) {
        diffs.push_back({pt.y - pt.x, pt});
    }
    std::sort(diffs.begin(), diffs.end());

    cv::Point topRight = diffs[0].second;
    cv::Point bottomLeft = diffs[3].second;

    ordered[0] = topLeft;
    ordered[1] = topRight;
    ordered[2] = bottomRight;
    ordered[3] = bottomLeft;

    return ordered;
}

cv::Mat AutoCrop::correctPerspective(const cv::Mat& image, const std::vector<cv::Point>& corners) {
    if (image.empty()) {
        qWarning() << "Auto-crop: correctPerspective - empty image";
        return image;
    }

    if (corners.size() != 4) {
        qWarning() << "Auto-crop: Need exactly 4 corners for perspective correction, got:" << corners.size();
        return image;
    }

    try {
        // Order the corners
        std::vector<cv::Point> ordered = orderPoints(corners);

        if (ordered.size() != 4) {
            qWarning() << "Auto-crop: Point ordering failed in correctPerspective";
            return image;
        }

        // Calculate the width of the new image
        double widthTop = cv::norm(ordered[1] - ordered[0]);
        double widthBottom = cv::norm(ordered[2] - ordered[3]);
        int maxWidth = static_cast<int>(std::max(widthTop, widthBottom));

        // Calculate the height of the new image
        double heightLeft = cv::norm(ordered[3] - ordered[0]);
        double heightRight = cv::norm(ordered[2] - ordered[1]);
        int maxHeight = static_cast<int>(std::max(heightLeft, heightRight));

        // Validate dimensions
        if (maxWidth <= 0 || maxHeight <= 0) {
            qWarning() << "Auto-crop: Invalid perspective dimensions:" << maxWidth << "x" << maxHeight;
            return image;
        }

        // Define destination points
        std::vector<cv::Point2f> dst = {
            cv::Point2f(0, 0),
            cv::Point2f(maxWidth - 1, 0),
            cv::Point2f(maxWidth - 1, maxHeight - 1),
            cv::Point2f(0, maxHeight - 1)
        };

        // Convert source points to Point2f
        std::vector<cv::Point2f> src;
        for (const auto& pt : ordered) {
            src.push_back(cv::Point2f(pt.x, pt.y));
        }

        // Get perspective transform matrix
        cv::Mat M;
        try {
            M = cv::getPerspectiveTransform(src, dst);
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed to get perspective transform:" << e.what();
            return image;
        }

        if (M.empty()) {
            qWarning() << "Auto-crop: Perspective transform matrix is empty";
            return image;
        }

        // Apply perspective transformation
        cv::Mat warped;
        try {
            cv::warpPerspective(image, warped, M, cv::Size(maxWidth, maxHeight));
        } catch (const cv::Exception& e) {
            qWarning() << "Auto-crop: Failed to warp perspective:" << e.what();
            return image;
        }

        if (warped.empty()) {
            qWarning() << "Auto-crop: Warped image is empty";
            return image;
        }

        qInfo() << "Auto-crop: Perspective correction successful:" << maxWidth << "x" << maxHeight;
        return warped;

    } catch (const cv::Exception& e) {
        qCritical() << "Auto-crop: OpenCV exception in correctPerspective:" << e.what();
        return image;
    } catch (const std::exception& e) {
        qCritical() << "Auto-crop: Standard exception in correctPerspective:" << e.what();
        return image;
    } catch (...) {
        qCritical() << "Auto-crop: Unknown exception in correctPerspective";
        return image;
    }
}

double AutoCrop::calculateArea(const std::vector<cv::Point>& contour) {
    return cv::contourArea(contour);
}
