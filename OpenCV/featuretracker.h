#ifndef FEATURE_TRACKER_H
#define FEATURE_TRACKER_H
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include "FrameProcessor.h"

class FeatureTracker : public FrameProcessor
{
private:

	cv::Mat gray;
	cv::Mat gray_prev;
	std::vector<cv::Point2f> points[2];
	std::vector<cv::Point2f> initial;
	std::vector<cv::Point2f> features;
	int max_count;
	double qlevel;
	double minDist;
	std::vector<uchar> status;
	std::vector<float> err;
	const static int max_frame_count = 15;
	int num_tracked[max_frame_count];
	int diff_tracked[max_frame_count];
	int acc_tracked[max_frame_count];
	void saveTrackedInfo(int);
	bool trySaveImage(cv::Mat& frame);
public:
	void process(cv::Mat &, cv::Mat &);
	FeatureTracker() : max_count(500), qlevel(0.01), minDist(10.){}
	void detectFeaturePoints();
	bool addNewPoints();
	bool acceptTrackedPoint(int i);
	void handleTrackedPoints(cv::Mat &frame, cv::Mat &output);	
	static int img_count;
};
#endif