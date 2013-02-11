#include "VideoProc.h"

void VideoProcessor::onTrackbarSlide(int current_frame, void* data)
{
	current_frame = slider_position;
	m_capture.set(CV_CAP_PROP_POS_FRAMES, current_frame);
}

void VideoProcessor::setFrameProcessor(
		void (*frameProcessingCallback)
		(cv::Mat&, cv::Mat&)){
		m_frameProcessor = 0;
		process = frameProcessingCallback;
		callProcess();
}

void VideoProcessor::setFrameProcessor(FrameProcessor* frameProcessorPtr)
{
	process = 0;
	m_frameProcessor = frameProcessorPtr;
	callProcess();
}

bool VideoProcessor::setInput(std::string filename){
	m_fnumber = 0;
	m_capture.release();
	//images.clear();
	return m_capture.open(filename);
}

void VideoProcessor::displayInput(std::string wn){
	m_windowNameInput = wn;
	cv::namedWindow(m_windowNameInput);
}

void VideoProcessor::displayOutput(std::string wn){
	m_windowNameOutput = wn;
	cv::namedWindow(m_windowNameOutput);
}

void VideoProcessor::dontDisplay(){
	cv::destroyWindow(m_windowNameInput);
	cv::destroyWindow(m_windowNameOutput);
	m_windowNameInput.clear();
	m_windowNameOutput.clear();
}

void VideoProcessor::run(){
	cv::Mat frame;
	cv::Mat output;
	if(!isOpened())
		return;
	m_stop = false;
	m_capture.set(CV_CAP_PROP_POS_FRAMES, 1.0);
	while(!isStopped()){
		if(!readNextFrame(frame))
			break;
		if(m_windowNameInput.length() != 0)
			cv::imshow(m_windowNameInput, frame);
		if(m_callIt){
			int no_of_frames = (int)m_capture.get(CV_CAP_PROP_FRAME_COUNT);
			if(no_of_frames != 0)
			{
				//cv::createTrackbar("Slider", m_windowNameInput, &slider_position, no_of_frames, onTrackbarSlide);
			}
			if(process)
			process(frame, output);
			else if(m_frameProcessor)
				m_frameProcessor->process(frame, output);
			m_fnumber++;
			cv::setTrackbarPos("Slider", m_windowNameInput, ++slider_position); 
		}else{
			output = frame;
		}
		if(m_windowNameOutput.length() != 0)
			cv::imshow(m_windowNameOutput, output);
		if(m_delay >= 0 && cv::waitKey(m_delay) >= 0)
			stopIt();
		if(m_frameToStop >= 0 &&  getFrameNumber() == m_frameToStop)
			stopIt();
	}
}

void VideoProcessor::stopIt(){
	m_stop = true;
}

bool VideoProcessor::isStopped(){
	return m_stop;
}

bool VideoProcessor::isOpened(){
	return m_capture.isOpened();
}

void VideoProcessor::setDelay(int d){
	m_delay = d;
}

bool VideoProcessor::readNextFrame(cv::Mat& frame){
	return m_capture.read(frame);
}

void VideoProcessor::callProcess(){
	m_callIt = true;
}

void VideoProcessor::dontCallProcess(){
	m_callIt = false;
}

void VideoProcessor::stopAtFrameNo(long frame){
	m_frameToStop = frame;
}

long VideoProcessor::getFrameNumber(){
	long fnumber = static_cast<long>(
						m_capture.get(CV_CAP_PROP_POS_FRAMES));
	return fnumber;
}

double VideoProcessor::getFrameRate(){
	return m_capture.get(CV_CAP_PROP_FPS);
}

