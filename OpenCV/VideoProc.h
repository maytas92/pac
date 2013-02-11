#ifndef VIDEOPROC_H
#define VIDEOPROC_H
// Video Image PSNR and SSIM
#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <vector>
#include <ctime>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include "FrameProcessor.h"

class VideoProcessor
{
private:
	cv::VideoCapture m_capture;
	FrameProcessor* m_frameProcessor;
	void (*process)(cv::Mat &, cv::Mat &);
	bool m_callIt;
	std::string m_windowNameInput;
	std::string m_windowNameOutput;
	int m_delay;
	long m_fnumber;
	long m_frameToStop;
	bool m_stop;
	int slider_position;
public:
	VideoProcessor():m_callIt(true), m_delay(0), m_fnumber(0), m_stop(false), m_frameToStop(-1){}
	void setFrameProcessor(
		void (*frameProcessingCallback)
		(cv::Mat&, cv::Mat&));	
	void setFrameProcessor(FrameProcessor* frameProcessorPtr);
	bool setInput(std::string filename);
	void displayInput(std::string wn);
	void displayOutput(std::string wn);
	void dontDisplay();
	void run();
	void stopIt();
	bool isStopped();
	bool isOpened();
	void setDelay(int d);
	bool readNextFrame(cv::Mat& frame);
	void callProcess();
	void dontCallProcess();
	void stopAtFrameNo(long frame);
	long getFrameNumber();
	double getFrameRate();
	void onTrackbarSlide(int, void*);
};



#endif
