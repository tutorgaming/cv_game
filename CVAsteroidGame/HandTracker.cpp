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
	resizeScale = 0.25f;
	invResizeScale = 1 / resizeScale;
	m_IsTracking = false;
}

void HandTracker::detectHand(Mat &inputImage, Rect &handRect)
{
	vector<Rect> hands;
	hand_cascade.detectMultiScale(inputImage, hands, 1.06, 3,CV_HAAR_FIND_BIGGEST_OBJECT, Size(60, 60));
	int maxArea = 0;
	Rect biggestRect;
	// Getting biggest hand rect
	if (hands.size() <= 0) {
		m_IsTracking = false;
		return;
	}
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
	m_IsTracking = true;
}

float* HandTracker::getHandPosition()
{
	float xPos = (lastHandRect.x + lastHandRect.width / 2 - m_FrameWidth / 2) / m_FrameWidth * 2;
	float yPos = -(lastHandRect.y + lastHandRect.height / 2 - m_FrameHeight / 2) / m_FrameHeight * 2;
	float* result = new float[2];
	result[0] = xPos;
	result[1] = yPos;
	return result;
}

void HandTracker::process(cv::Mat &input)
{
	m_FrameWidth = input.cols;
	m_FrameHeight = input.rows;

	Mat resizedImg;
	//resize(input, resizedImg, cv::Size(input.cols * resizeScale, input.rows * resizeScale));
	// Convert to YCrCb
	Mat yCrCbImg;
	cvtColor(input, yCrCbImg, CV_BGR2YCrCb);
	// In-range
	inRange(yCrCbImg, Scalar(80, 135, 85), Scalar(255, 180, 135), yCrCbImg);
	
	// Detect-Hand
	Rect resultRect;
	detectHand(yCrCbImg, lastHandRect);

}