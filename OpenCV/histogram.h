#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

class Histogram1D
{
private: 
	int m_histSize[1]; // number of bins
	float m_hranges[2]; // min and max pixel value
	const float* m_ranges[1];
	int m_channels[1];
public:
	Histogram1D();
	// Computing 1D Historgram i.e. for a grayscale image
	cv::MatND getHistogram(const cv::Mat &image);
	cv::MatND Histogram1D::getNormalizedHistogram(const cv::Mat &image);
	cv::Mat getHistogramImage(const cv::Mat &image);
	int getHistSize();
	cv::SparseMat getSparseHistogram(const cv::Mat &image);
	cv::Mat applyLookUp(const cv::Mat &image, const cv::Mat &lookup);
	cv::Mat stretch(const cv::Mat &image, int minValue = 0);
};

#endif