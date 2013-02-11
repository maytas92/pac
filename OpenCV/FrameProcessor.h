#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

class FrameProcessor
{
public:
	virtual void process(cv::Mat &input, cv::Mat &output)=0;
	void setFrameProcessor(FrameProcessor* frameProcessorPtr);
	void setFrameProcessor(void (*frameProcessingCallback)(cv::Mat&, cv::Mat&));
};

#endif
