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
	float resizeScale;
	float invResizeScale;
	int m_FrameWidth;
	int m_FrameHeight;
	bool m_IsTracking;
public:

private:
	void detectHand(Mat &inputImage, Rect &handRect);
	void trackHand(Mat &inputImage, Rect &handRect);
public:
	HandTracker(void);
	virtual ~HandTracker(void);
	void init(void);
	void process(cv::Mat &input);
	Rect getLastHandRect() { return lastHandRect; };
	float* getHandPosition();
	bool isTracking() { return m_IsTracking; };
};