#ifndef CONTOUR_FINDER_H
#define CONTOUR_FINDER_H

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)

class ContourFinder
{
private:
public:
	void findContours(cv::Mat& image, std::vector<std::vector<cv::Point> > contours);
	void selectContours(int cMin = 100, int cMax = 1000);
};

#endif