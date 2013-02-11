#include "VideoProc.h"
#include "Histogram.h"
#include "FeatureTracker.h"
#include "FrameProcessor.h"

#include <cstdio>

#ifdef _DEBUG
#pragma comment(lib, "opencv_highgui231d")
#pragma comment(lib, "opencv_calib231d")
#pragma comment(lib, "opencv_contrib231d")

#else
#pragma comment(lib, "opencv_highgui231")
#pragma comment(lib, "opencv_calib3d231")
#pragma comment(lib, "opencv_contrib231")
#pragma comment(lib, "opencv_core231")
#pragma comment(lib, "opencv_features2d231")
#pragma comment(lib, "opencv_flann231")
#pragma comment(lib, "opencv_gpu231")
#pragma comment(lib, "opencv_haartraining_engine")
#pragma comment(lib, "opencv_imgproc231")
#pragma comment(lib, "opencv_legacy231")
#pragma comment(lib, "opencv_ml231")
#pragma comment(lib, "opencv_objdetect231")
#pragma comment(lib, "opencv_ts231")
#pragma comment(lib, "opencv_video231")
#endif


int func_histogram(std::string file_name, int start_count, int &func_histogram_count)
{
	Histogram1D h;
    cv::Mat frame, prevFrame;
	cv::Mat back;
    cv::Mat fore;
    cv::VideoCapture video_capture(file_name);
    cv::BackgroundSubtractorMOG2 bg;
    bg.nmixtures = 3;
    bg.bShadowDetection = false;
 
    std::vector<std::vector<cv::Point> > contours;
 
    cv::namedWindow("Video");
	std::string album_name = file_name;
	char str[255];
	clock_t start;
	double diff = 0.0;
	double time_diff_crit = 1.0;
	int prev_diff = 0;
	double rate = video_capture.get(CV_CAP_PROP_FPS);
	int fps = 1000/rate;
	double histMatch = 0.0;
	double prevHistMatch = 0.0;

	cv::MatND frameHist, prevFrameHist;
	cv::Mat edges;
	cv::Mat sub, subGray;
	cv::Mat frameGray, prevFrameGray;
	int threshold = 1000;
	bool first_frame = true;	
	bool toggle_img_capture = false;
	bool saved_image = false;
    for(;;)
    {		
		int no_of_frames = (int)video_capture.get(CV_CAP_PROP_FRAME_COUNT);

        video_capture >> prevFrame;
		if(!prevFrame.data)
			break;
		video_capture >> frame;
		if(!frame.data)
			break;
		frameGray = cv::Mat(frame.size(), 1);
		prevFrameGray = cv::Mat(prevFrame.size(), 1);
		cv::cvtColor(frame, frameGray, CV_RGB2GRAY);
		cv::cvtColor(prevFrame, prevFrameGray, CV_RGB2GRAY);
		frameHist = h.getNormalizedHistogram(frame);
		prevFrameHist = h.getNormalizedHistogram(prevFrame);
		prevHistMatch = histMatch;
		histMatch = cv::compareHist(frameHist, prevFrameHist, CV_COMP_INTERSECT);
		std::cout << "Hist match diff " << prevHistMatch - histMatch << std::endl;
		std::cout << "Hist Match " << histMatch << std::endl;

		if((prevHistMatch - histMatch) > 1.0 || (prevHistMatch - histMatch) < -1.0 ){
			if(func_histogram_count == 0)
			{
				sprintf(str, "pic%d.jpg", start_count + func_histogram_count);
				cv::imwrite(str, frame);
				cv::Mat histShow = h.getHistogramImage(frame);
				//sprintf(str, "hist%d", start_count + func_histogram_count);
				//cv::imshow(str, histShow);
				func_histogram_count++;
			}
			if(!toggle_img_capture){
				saved_image = false;
				start = std::clock();		
				printf("Start is %f\n", start);
			}
			else
			{				
				saved_image = true;
				diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
				printf("Diff is %f\n", diff);
				if(diff > 1.0 || func_histogram_count == 0){
					sprintf(str, "pic%d.jpg", start_count + func_histogram_count);
					cv::imwrite(str, frame);
					cv::Mat histShow = h.getHistogramImage(frame);
					//sprintf(str, "hist%d", start_count + func_histogram_count);
					//cv::imshow(str, histShow);
					func_histogram_count++;
					saved_image = false;
				}
			}			
			if(!saved_image)
				toggle_img_capture = !toggle_img_capture;
		}

        bg.operator ()(frame,fore);
        bg.getBackgroundImage(back);
        cv::erode(fore,fore,cv::Mat());
        cv::dilate(fore,fore,cv::Mat());		

        cv::findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        cv::drawContours(frame, contours, -1, cv::Scalar(0,0,255), 2);
        cv::imshow("Video", frame);

        if(cv::waitKey(fps) >= 0) break;
    }
	cv::waitKey(0);
    return 0;
}

bool func_pac(char* video_file)
{
	Histogram1D h;
	// Create video procesor instance
	VideoProcessor processor;
	// Create feature tracker instance
	FeatureTracker tracker;
	std::string file_name = std::string(video_file);
	int fh_img_count = 0;
	// Open video file
	processor.setInput(file_name);
	// set frame processor
	processor.setFrameProcessor(&tracker);
	// Declare a window to display the video
	processor.displayOutput("Tracked Features");
	// Play the video at the original frame rate
	processor.setDelay(1000./processor.getFrameRate());
	// Start the process
	processor.run();
	
	func_histogram(file_name, FeatureTracker::img_count, fh_img_count);
	
	char str[255];
	cv::Mat img;
	cv::Mat prev, prev_gray;
	cv::Mat curr, curr_gray;
	cv::Mat sub;	
	cv::MatND frameHist, prevFrameHist;
	double histMatch = 0.0;
	int *img_to_delete = new int[FeatureTracker::img_count + fh_img_count];
	int file_count = 0;
	
	for(int i = 0; i < FeatureTracker::img_count + fh_img_count; i++)
	{
		sprintf(str, "pic%d.jpg", i);
		img = cv::imread(str, 1);
		if(!img.data){
			printf("Could not read image %d\n", i);
			continue;
		}
		prev = img;
		cv::cvtColor(prev, prev_gray, CV_RGB2GRAY);		
		for(int j = i + 1; j < FeatureTracker::img_count + fh_img_count; j++){
			sprintf(str, "pic%d.jpg", j);
			img = cv::imread(str, 1);
			if(!img.data){
				printf("Could not read image %d\n", j);
				continue;
			}
			curr = img;
			cv::cvtColor(curr, curr_gray, CV_RGB2GRAY);
			frameHist = h.getNormalizedHistogram(curr_gray);
			prevFrameHist = h.getNormalizedHistogram(prev_gray);
			int nonZero = cv::countNonZero(sub);
			histMatch = cv::compareHist(frameHist, prevFrameHist, CV_COMP_INTERSECT);
			printf("Hist match for images %d and %d is %f\n", i, j, histMatch);
			
			if(histMatch > 8.8){ // based on a heuristic..
				printf("Similar Images Detected\n");
				
				bool isMarked = false;
				for(int k=0; k < file_count; k++)
				{
					if(img_to_delete[k] == j)
						isMarked = true;
				}
				if(!isMarked)
					img_to_delete[file_count++] = j;				
			}else{
				printf("File not deleted\n");
			}		
		}	
	}
	for(int i = 0; i < file_count; i++)
	{
		sprintf(str, "pic%d.jpg", img_to_delete[i]);
		if(std::remove(str) != 0){
			std::perror("Error deleting file\n");
		}
		else
		{
			std::puts("File successfully deleted");
		}
	}
	delete [] img_to_delete;

	cv::waitKey(0);
	return true;
}

int main(int argc, char* argv[])
{
	if(argc == 0)
	{
		std::cout << "Please provide a video file name as input\n";
		return 0;
	}
	if(func_pac(argv[1]))
		return 0;
	return -1;
}
