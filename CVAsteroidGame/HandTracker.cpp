#include "HandTracker.hpp"

using namespace cv;

HandTracker::HandTracker(void)
{

}

HandTracker::~HandTracker(void)
{

}

void HandTracker::init(void)
{
	hand_cascade.load("HandCascade1.xml");
}

void HandTracker::detectHand(Mat &inputImage, Rect &handRect)
{
	vector<Rect> hands;
	hand_cascade.detectMultiScale(inputImage, hands, 1.06, 15);
	int maxArea = 0;
	Rect biggestRect;
	// Getting biggest hand rect
	for (int i = 0; i < hands.size(); i++){
		Rect hRect = hands[i];
		int area = hRect.area();
		if (area > maxArea)
		{
			maxArea = area;
			biggestRect = hRect;
		}
	}
	handRect = biggestRect;
}


void HandTracker::process(cv::Mat &input, cv::Mat &output)
{
	output = input.clone();
	// Convert to YCrCb
	Mat yCrCbImg;
	cvtColor(input, yCrCbImg, CV_BGR2YCrCb);
	// In-range
	inRange(yCrCbImg, Scalar(80, 135, 85), Scalar(255, 180, 135), yCrCbImg);
	// Detect-Hand
	detectHand(yCrCbImg, lastHandRect);


}