#include "featuretracker.h"

int FeatureTracker::img_count = 0;
static int frame_counter = 0;

void FeatureTracker::process(cv::Mat &frame, cv::Mat &output)
{
	if(!frame.data)
		printf("Error in input frame\n");
	cv::cvtColor(frame, gray, CV_BGR2GRAY);
	if(!gray.data)
		printf("Error in cvt color\n");
	frame.copyTo(output);
	// 1. if new feature points must be added
	if(addNewPoints())
	{
		// detect feature points
		detectFeaturePoints();
		// add the detected features to
		// the currently tracked features
		points[0].insert(points[0].end(),
		features.begin(),features.end());
		initial.insert(initial.end(),
		features.begin(),features.end());
	}
	// for first image of the sequence
	if(gray_prev.empty())
		gray.copyTo(gray_prev);
	// 2. track features
	try{
	cv::calcOpticalFlowPyrLK(
		gray_prev, gray, // 2 consecutive images
		points[0], // input point positions in first image
		points[1], // output point positions in the 2nd image
		status, // tracking success
		err); // tracking error
	}
	catch(int e)
	{
		printf("There was an error %d in Optical flow\n", e);	
	}// 2. loop over the tracked points to reject some
	int k=0;
	for( int i= 0; i < points[1].size(); i++ ) {
		// do we keep this point?
		if (acceptTrackedPoint(i)) {
			// keep this point in vector
			initial[k]= initial[i];
			points[1][k++] = points[1][i];
		}
	}	
	saveTrackedInfo(k);
	// eliminate unsuccesful points
	points[1].resize(k);
	initial.resize(k);
	// 3. handle the accepted tracked points
	handleTrackedPoints(frame, output);
	// 4. current points and image become previous ones
	std::swap(points[1], points[0]);
	cv::swap(gray_prev, gray);
	if(frame_counter == max_frame_count)
		if(trySaveImage(frame))
			printf("Saved Image\n");
}

bool FeatureTracker::trySaveImage(cv::Mat& frame)
{
	char str[255];
	bool point_threshold = true;
	bool diff_threshold = true;
	bool static_image = true;
	for(int i = 0; i < max_frame_count; i++)
	{
		if(num_tracked[i] >= 125)
			continue;
		else{
			point_threshold = false;
			break;
		}
	}
	if(!point_threshold){
		frame_counter = 0;
		return false;
	}
	for(int i = 0; i < max_frame_count; i++)
	{
		if(diff_tracked[i] >= 100)
			continue;
		else{
			diff_threshold = false;
			break;
		}
	}
	for(int i = 0; i < max_frame_count; i++)
	{
		if(acc_tracked[i] <= 10)
			continue;
		else{
			static_image = false;
			break;
		}
	}
	// Always need to reset frame counter once max_frame_count frames have been read by the video proc		
	frame_counter = 0;
	if(diff_threshold || static_image){
		sprintf(str, "pic%d.jpg", FeatureTracker::img_count++);
		cv::imwrite(str, frame);
		return true;
	}
	return false;
}

void FeatureTracker::saveTrackedInfo(int resize)
{
	if(resize <= 10)
	{
		num_tracked[frame_counter] = points[1].size();
		diff_tracked[frame_counter] = points[1].size() - resize;
		acc_tracked[frame_counter] = resize;		
		printf("The size of the initial points tracked %d\n", num_tracked[frame_counter]);		
		printf("The size of the diff tracked points %d\n", diff_tracked[frame_counter]);		
		printf("The size of the accepted tracked points is %d\n", acc_tracked[frame_counter]);
		printf("The frame counter is %d\n", frame_counter);
		frame_counter++;
	}
	else
	{
		frame_counter = 0;
	}
}

// feature point detection
void FeatureTracker::detectFeaturePoints() {
	// detect the features
	cv::goodFeaturesToTrack(gray, // the image
		features, // the output detected features
		max_count, // the maximum number of features
		qlevel, // quality level
		minDist); // min distance between two features
}

//Determines if new points are to be added
bool FeatureTracker::addNewPoints() {
	// if too few points
	return points[0].size()<=10;
}

// determine which tracked point should be accepted
bool FeatureTracker::acceptTrackedPoint(int i) {
	return status[i] &&
	// if point has moved
	(abs(points[0][i].x-points[1][i].x)+
	(abs(points[0][i].y-points[1][i].y))>2);
}

// handle the currently tracked points
void FeatureTracker::handleTrackedPoints(cv:: Mat &frame,
cv:: Mat &output) {
	// for all tracked points
	for(int i= 0; i < points[1].size(); i++ ) {
		// draw line and circle
		cv::line(output,
		initial[i], // initial position
		points[1][i],// new position
		cv::Scalar(255,255,255));
		cv::circle(output, points[1][i], 3,
		cv::Scalar(255,255,255),-1);
	}
}