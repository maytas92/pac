#include "histogram.h"

// Constructor
Histogram1D::Histogram1D(){
	m_histSize[0] = 256;
	m_hranges[0] = 0.0;
	m_hranges[1] = 255;
	m_ranges[0] = m_hranges;
	m_channels[0] = 0; // look at channel 0
}

cv::SparseMat Histogram1D::getSparseHistogram(const cv::Mat &image){
	cv::SparseMat  hist(3, m_histSize, CV_32F);
	cv::calcHist(&image, 
					1, 
					m_channels, 
					cv::Mat(),
					hist, 
					1, 
					m_histSize, 
					m_ranges
				);
	return hist;
}
// Get a histogram for 1 channel grayscale image
cv::MatND Histogram1D::getHistogram(const cv::Mat &image){
		cv::MatND hist;
		cv::calcHist(&image, 1, m_channels, cv::Mat(), hist, 1, m_histSize, m_ranges);
		return hist;
}

cv::MatND Histogram1D::getNormalizedHistogram(const cv::Mat &image){
		cv::MatND hist;
		cv::calcHist(&image, 1, m_channels, cv::Mat(), hist, 1, m_histSize, m_ranges);
		cv::normalize(hist, hist);
		return hist;
}



cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image){
	// Compute histogram first
	cv::MatND hist= getHistogram(image);
	// Get min and max bin values
	double maxVal=0;
	double minVal=0;
	cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);
	// Image on which to display histogram
	cv::Mat histImg(m_histSize[0], m_histSize[0],
	CV_8U,cv::Scalar(255));
	// set highest point at 90% of nbins
	int hpt = static_cast<int>(0.9*m_histSize[0]);
	// Draw a vertical line for each bin
	for( int h = 0; h < m_histSize[0]; h++ ) {
	float binVal = hist.at<float>(h);
	int intensity = static_cast<int>(binVal*hpt/maxVal);
	// This function draws a line between 2 points
	cv::line(histImg,cv::Point(h,m_histSize[0]),
	cv::Point(h,m_histSize[0]-intensity),
	cv::Scalar::all(0));
	}
	return histImg;
}

cv::Mat Histogram1D::applyLookUp(const cv::Mat &image, const cv::Mat &lookup){
	cv::Mat result;
	cv::LUT(image, lookup, result);
	return result;
}
cv::Mat Histogram1D::stretch(const cv::Mat &image, int minValue){
	cv::MatND hist = getHistogram(image);
	int imin = 0;
	for( ; imin < m_histSize[0]; imin++){
		//std::cout << hist.at<float>(imin) << std::endl;
		if( hist.at<float>(imin) > minValue )
			break;
	}
	int imax = m_histSize[0] - 1;
	for( ; imax >= 0; imax-- ){
		if(hist.at<float>(imax) > minValue )
			break;
	}

	int dim(256);
	cv::Mat lookup(1, 
					&dim, 
					CV_8U
					);
	for(int i = 0; i < 256; i++){
		if( i < imin ) lookup.at<uchar>(i) = 0;
		else if ( i > imax ) lookup.at<uchar>(i) = 255;
		else lookup.at<uchar>(i) = static_cast<uchar>(
											255.0*(i - imin)/ (imax-imin) + 0.5);

	}
	cv::Mat result;
	result = applyLookUp(image, lookup);
	return result;
}