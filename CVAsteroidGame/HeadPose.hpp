#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>


//---------------------------------------------------------------------------
using namespace cv;

class HeadPose
{

private:
	cv::CascadeClassifier face_cascade, nose_cascade, eye_cascade, left_eye_cascade, right_eye_cascade, mouth_cascade;
	std::vector<CvPoint3D32f> modelPoints;
	std::vector<CvPoint3D32f> axesPoints;
	std::vector<CvPoint2D32f> srcImagePoints;
	CvPOSITObject* positObject;
	float* rotation_matrix;
	float* translation_vector;
	CvTermCriteria criteria;
	std::vector<CvPoint2D32f> projectedPoints;
	std::vector<CvPoint2D32f> projectedOriginalPoints;
	CvPoint2D32f m_lastHeadPosition;
	int m_FrameHeight;
	int m_FrameWidth;
	Mat faceTemplate, noseTemplate, leftEyeTemplate, rightEyeTemplate, mouthTemplate;
	Mat lastNoseMatch, lastLeftEyeMatch, lastRightEyeMatch, lastMouthMatch;
	Rect lastNoseRect, lastLeftEyeRect, lastRightEyeRect, lastMouthRect;
	bool m_foundFace;
	int match_method;
	double distThreshold;
	float* lookPosition;
	KalmanFilter KF;
public:
	HeadPose(void);
	virtual ~HeadPose(void);
	void init(void);
	float* getHeadRotationMatrix(void);
	float* getHeadPosition();
	float* getLookPosition();

private:
	void detectFaceFeatures(Mat &inputImage);
	void trackFaceFeatures(Mat &inputImage);
	Rect getMatchingRect(Mat &inputImage, Mat &templateImg);
	int getDistance2(Rect &rect1, Rect &rect2);
	Rect getBestMatching(Mat &inputImage, Rect &lastRect, Mat &lastMatch, Mat &oriTemplate );

public:
	void process(cv::Mat &input, cv::Mat &output);

};