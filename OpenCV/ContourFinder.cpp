#include "ContourFinder.h"

void ContourFinder::findContours(cv::Mat &image, std::vector<std::vector<cv::Point> > contours)
{
	cv::Mat image_gray;
	cv::cvtColor(image, image_gray, CV_RGB2GRAY);
	cv::findContours(image_gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
}