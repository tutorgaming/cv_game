#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>


//---------------------------------------------------------------------------
using namespace cv;

class HandTracker
{

private:
	CascadeClassifier	hand_cascade;
	Rect lastHandRect;
public:

private:
	void detectHand(Mat &inputImage, Rect &handRect);
	void trackHand(Mat &inputImage, Rect &handRect);
public:
	HandTracker(void);
	virtual ~HandTracker(void);
	void init(void);
	void process(cv::Mat &input, cv::Mat &output);
	Rect getLastHandRect() { return lastHandRect; };

};