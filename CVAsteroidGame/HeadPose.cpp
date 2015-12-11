#include "HeadPose.hpp"

#define FOCAL_LENGTH 1000
#define CUBE_SIZE 10
#define AXIS_SIZE 100

using namespace cv;

HeadPose::HeadPose(void)
{

}

HeadPose::~HeadPose(void)
{
	delete rotation_matrix;
	delete translation_vector;
	delete lookPosition;
	cvReleasePOSITObject(&positObject);
}


void HeadPose::init(void)
{
	face_cascade.load("haar-face.xml");
	nose_cascade.load("Nariz_nuevo_20stages.xml");
	left_eye_cascade.load("haar_l_eye.xml");
	right_eye_cascade.load("haar_r_eye.xml");
	eye_cascade.load("haar_r_eye.xml");
	mouth_cascade.load("Mouth.xml");
	

	//Create the model points
	cv::Point3f nosePoint(-0.0697709f, 11.3f, 161.2f);
	modelPoints.push_back(nosePoint); //nose (origin)
	modelPoints.push_back(cv::Point3f(-37.02197f, 45.93336f, 134.2403f));    //l eye
	modelPoints.push_back(cv::Point3f(35.37623f, 44.91002f, 134.3533f));      //r eye
	modelPoints.push_back(cv::Point3f(-27.71367f, -25.0457f, 125.0767f));   //l mouth
	modelPoints.push_back(cv::Point3f(28.70953f, -24.25141f, 124.217f));    //r mouth
	//modelPoints.push_back(Point3f(-87.2155f,15.5829f,-45.1352f) - nosePoint);   //l ear
	//modelPoints.push_back(Point3f(85.8383f,14.9023f,-46.3169f) - nosePoint);    //r ear

	// Create axes points
	axesPoints.push_back(cv::Point3f(0, 0, 0));
	axesPoints.push_back(cv::Point3f(AXIS_SIZE, 0, 0)); // X
	axesPoints.push_back(cv::Point3f(0, AXIS_SIZE, 0)); // Y
	axesPoints.push_back(cv::Point3f(0, 0, AXIS_SIZE)); // Z

	//Initialize etc
	rotation_matrix = new float[9];
	translation_vector = new float[3];
	lookPosition = new float[2];
	lookPosition[0] = 0;
	lookPosition[1] = 0;
	criteria = cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 1.0e-4f);

	m_foundFace = false;
	match_method = CV_TM_SQDIFF;

	//Kalman Filter
	KF = KalmanFilter(4, 2, 0);
	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);

}

void project(CvPoint3D32f &input, CvPoint2D32f &output)
{
	if (input.z != 0)
	{
		output.x = FOCAL_LENGTH * input.x / input.z;
		output.y = FOCAL_LENGTH * input.y / input.z;
	}
	else {
		output.x = input.x;
		output.y = input.y;
	}
}

void transformAndProject(CvPoint3D32f &input, CvPoint2D32f &output, float* rotation_matrix, float* translation_vector)
{
	CvPoint3D32f point3D;
	point3D.x = rotation_matrix[0] * input.x +
		rotation_matrix[1] * input.y +
		rotation_matrix[2] * input.z +
		translation_vector[0];
	point3D.y = rotation_matrix[3] * input.x +
		rotation_matrix[4] * input.y +
		rotation_matrix[5] * input.z +
		translation_vector[1];
	point3D.z = rotation_matrix[6] * input.x +
		rotation_matrix[7] * input.y +
		rotation_matrix[8] * input.z +
		translation_vector[2];
	CvPoint2D32f point2D = cvPoint2D32f(0.0, 0.0);
	if (point3D.z != 0)
	{
		output.x = FOCAL_LENGTH * point3D.x / point3D.z;
		output.y = FOCAL_LENGTH * point3D.y / point3D.z;
	}
}

void HeadPose::detectFaceFeatures(Mat &inputImage)
{
	Mat showImage = inputImage.clone();
	/// Detect face
	vector<Rect> faces;
	face_cascade.detectMultiScale(inputImage, faces, 1.1, 5);
	if (faces.size() > 0) /// Found face
	{
		Rect faceRect = faces[0];
		faceTemplate = Mat(inputImage, faceRect);
		rectangle(showImage, faceRect, Scalar(255, 0, 0));

		/// Find nose
		vector<Rect> noses;
		nose_cascade.detectMultiScale(faceTemplate, noses, 1.1, 10);
		if (noses.size() > 0)
		{
			noseTemplate = Mat(faceTemplate, noses[0]);

			Mat face_roi = Mat(inputImage, faceRect);

			Rect noseRect = noses[0];
			rectangle(showImage, Rect(faceRect.x + noseRect.x, faceRect.y + noseRect.y, noseRect.width, noseRect.height), Scalar(255, 0, 0));

			int start_top_y = (int)(0.3*faceRect.height);
			int top_height = noseRect.y + noseRect.height / 2 - start_top_y;
			Rect top_left_rect = Rect(0, start_top_y, noseRect.x + noseRect.width, top_height);
			Mat face_top_left_roi(face_roi, top_left_rect);

			int tr_width = faceRect.width - noseRect.x;
			Rect top_right_rect = Rect(faceRect.width - tr_width, start_top_y, tr_width, top_height);
			Mat face_top_right_roi(face_roi, top_right_rect);

			int b_height = faceRect.height - (noseRect.y + (int)(noseRect.height*0.67));
			Rect bottom_rect = Rect(0, faceRect.height - b_height, faceRect.width, b_height);
			Mat face_bottom_roi(face_roi, bottom_rect);

			/// Find left eye
			vector<Rect> l_eyes;
			left_eye_cascade.detectMultiScale(face_top_left_roi, l_eyes, 1.05, 18);
			if (l_eyes.size() > 0)
			{
				Rect leftEyeRect = l_eyes[0];
				leftEyeTemplate = Mat(face_top_left_roi, leftEyeRect);
				rectangle(showImage, Rect(faceRect.x + top_left_rect.x + leftEyeRect.x, faceRect.y + top_left_rect.y + leftEyeRect.y, leftEyeRect.width, leftEyeRect.height), Scalar(255, 0, 0));

				/// Find right eye
				vector<Rect> r_eyes;
				right_eye_cascade.detectMultiScale(face_top_right_roi, r_eyes, 1.05, 18);
				if (r_eyes.size() > 0)
				{
					Rect rightEyeRect = r_eyes[0];
					rightEyeTemplate = Mat(face_top_right_roi, rightEyeRect);
					rectangle(showImage, Rect(faceRect.x + top_right_rect.x + rightEyeRect.x, faceRect.y + top_right_rect.y + rightEyeRect.y, rightEyeRect.width, rightEyeRect.height), Scalar(255, 0, 0));

					/// Find mouth
					vector<Rect> mouths;
					mouth_cascade.detectMultiScale(face_bottom_roi, mouths, 1.05, 6);
					if (mouths.size() > 0)
					{
						Rect mouthRect = mouths[0];
						mouthTemplate = Mat(face_bottom_roi, mouthRect);
						rectangle(showImage, Rect(faceRect.x + bottom_rect.x + mouthRect.x, faceRect.y + bottom_rect.y + mouthRect.y, mouthRect.width, mouthRect.height), Scalar(255, 0, 0));

						if (waitKey(10) == 'o') {
							destroyWindow("Detected Face features");
							m_foundFace = true;

							lastNoseMatch = noseTemplate.clone();
							lastNoseRect = noseRect;

							lastLeftEyeMatch = leftEyeTemplate.clone();
							lastLeftEyeRect = leftEyeRect;

							lastRightEyeMatch = rightEyeTemplate.clone();
							lastRightEyeRect = rightEyeRect;

							lastMouthMatch = mouthTemplate.clone();
							lastMouthRect = mouthRect;

							distThreshold = (inputImage.cols*inputImage.cols + inputImage.rows*inputImage.rows) / 1280;

							// Init Kalman filter
							KF.statePre.at<float>(0) = 0;
							KF.statePre.at<float>(1) = 0;
							KF.statePre.at<float>(2) = 0;
							KF.statePre.at<float>(3) = 0;
							setIdentity(KF.measurementMatrix);
							setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
							setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
							setIdentity(KF.errorCovPost, Scalar::all(.1));
						}
					}
				}
			}

		}
	}
	if (!m_foundFace)
	{
		imshow("Detected Face features", showImage);
	}
}

int HeadPose::getDistance2(Rect &rect1, Rect &rect2)
{
	int dx = rect1.x - rect2.x;
	int dy = rect1.y - rect2.y;
	return dx*dx + dy*dy;
}

Rect HeadPose::getBestMatching(Mat &inputImage, Rect &lastRect, Mat &lastMatch, Mat &oriTemplate)
{
	Rect newRect = getMatchingRect(inputImage, oriTemplate);
	int dist = getDistance2(newRect, lastRect);
	if (dist > distThreshold)
	{
		// Incorrect, get nearest to last match
		newRect = getMatchingRect(inputImage, lastMatch);
	}
	return newRect;
}

Rect HeadPose::getMatchingRect(Mat &inputImage, Mat &templateImg)
{
	Mat result;
	matchTemplate(inputImage, templateImg, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	return Rect(matchLoc.x, matchLoc.y, templateImg.cols, templateImg.rows);
}

void HeadPose::trackFaceFeatures(Mat &inputImage)
{
	Mat result;

	Rect faceRect = getMatchingRect(inputImage, faceTemplate);
	Mat face_roi = Mat(inputImage, faceRect);

	Rect noseRect = getBestMatching(face_roi, lastNoseRect, lastNoseMatch, noseTemplate);

	Rect top_left_rect = Rect(0, 0, max(noseRect.x + noseRect.width, leftEyeTemplate.cols), max(noseRect.y + noseRect.height, leftEyeTemplate.rows));
	Mat face_top_left_roi(face_roi, top_left_rect);
	Rect leftEyeRect = getBestMatching(face_top_left_roi, lastLeftEyeRect, lastLeftEyeMatch, leftEyeTemplate);

	int tr_width = max(rightEyeTemplate.cols, faceRect.width - noseRect.x);
	Rect top_right_rect = Rect(faceRect.width - tr_width, 0, tr_width, max(noseRect.y + noseRect.height, rightEyeTemplate.rows));
	Mat face_top_right_roi(face_roi, top_right_rect);
	Rect rightEyeRect = getBestMatching(face_top_right_roi, lastRightEyeRect, lastRightEyeMatch, rightEyeTemplate);

	int b_height = max(mouthTemplate.rows, faceRect.height - (noseRect.y + (int)(noseRect.height*0.67)));
	Rect bottom_rect = Rect(0, faceRect.height - b_height, faceRect.width, b_height);
	Mat face_bottom_roi(face_roi, bottom_rect);
	Rect mouthRect = getBestMatching(face_bottom_roi, lastMouthRect, lastMouthMatch, mouthTemplate);

	lastNoseRect = noseRect;
	lastNoseMatch = Mat(face_roi, noseRect).clone();

	lastLeftEyeRect = leftEyeRect;
	lastLeftEyeMatch = Mat(face_top_left_roi, leftEyeRect).clone();

	lastRightEyeRect = rightEyeRect;
	lastRightEyeMatch = Mat(face_top_right_roi, rightEyeRect).clone();

	lastMouthRect = mouthRect;
	lastMouthMatch = Mat(face_bottom_roi, mouthRect).clone();


	rectangle(inputImage, faceRect, Scalar::all(0), 2, 8, 0);
	rectangle(face_roi, top_left_rect, Scalar::all(0), 1, 8, 0);
	rectangle(face_roi, top_right_rect, Scalar::all(0), 1, 8, 0);
	rectangle(face_roi, noseRect, Scalar::all(0), 2, 8, 0);
	rectangle(face_top_left_roi, leftEyeRect, Scalar::all(0), 2, 8, 0);
	rectangle(face_top_right_roi, rightEyeRect, Scalar::all(0), 2, 8, 0);
	rectangle(face_bottom_roi, mouthRect, Scalar::all(0), 2, 8, 0);

	// Update last head position
	m_lastHeadPosition = cvPoint2D32f(faceRect.x + faceRect.width / 2, faceRect.y + faceRect.height / 2);

	//Update src points
	srcImagePoints.clear();

	Rect faceNoseRect = Rect(faceRect.x + noseRect.x, faceRect.y + noseRect.y, noseRect.width, noseRect.height);
	Rect faceLEyeRect = Rect(faceRect.x + top_left_rect.x + leftEyeRect.x, faceRect.y + top_left_rect.y + leftEyeRect.y, leftEyeRect.width, leftEyeRect.height);
	Rect faceREyeRect = Rect(faceRect.x + top_right_rect.x + rightEyeRect.x, faceRect.y + top_right_rect.y + rightEyeRect.y, rightEyeRect.width, rightEyeRect.height);
	Rect faceMouthRect = Rect(faceRect.x + bottom_rect.x + mouthRect.x, faceRect.y + bottom_rect.y + mouthRect.y, mouthRect.width, mouthRect.height);

	srcImagePoints.push_back(cvPoint2D32f(faceNoseRect.x + faceNoseRect.width / 2, faceNoseRect.y + faceNoseRect.height / 2)); // Move nose to top
	srcImagePoints.push_back(cvPoint2D32f(faceLEyeRect.x + faceLEyeRect.width / 2, faceLEyeRect.y + faceLEyeRect.height / 2));
	srcImagePoints.push_back(cvPoint2D32f(faceREyeRect.x + faceREyeRect.width / 2, faceREyeRect.y + faceREyeRect.height / 2));
	srcImagePoints.push_back(cvPoint2D32f(faceMouthRect.x, faceMouthRect.y + faceMouthRect.height / 2));
	srcImagePoints.push_back(cvPoint2D32f(faceMouthRect.x + faceMouthRect.width, faceMouthRect.y + faceMouthRect.height / 2));

	imshow("Face track", inputImage);
}

float* HeadPose::getHeadRotationMatrix(void)
{
	return rotation_matrix;
}

float* HeadPose::getLookPosition()
{
	return lookPosition;
}

float* HeadPose::getHeadPosition() // return -1 to 1
{
	float xPos = (m_lastHeadPosition.x - m_FrameWidth / 2) / m_FrameWidth * 2;
	float yPos = -(m_lastHeadPosition.y - m_FrameHeight / 2) / m_FrameHeight * 2;
	float* result = new float[2];
	result[0] = xPos;
	result[1] = yPos;
	return result;
}



void HeadPose::process(cv::Mat &input, cv::Mat &output)
{

	if (input.data) {
		m_FrameWidth = input.cols;
		m_FrameHeight = input.rows;
		output = input.clone();

		Mat grayImg;

		cvtColor(input, grayImg, COLOR_BGR2GRAY);

		if (!m_foundFace) /// Detect face features using Haar-cascade
		{
			detectFaceFeatures(grayImg);
		}
		else /// Track face features using template matching
		{
			trackFaceFeatures(grayImg);

			//Create the POSIT object with the model points
			positObject = cvCreatePOSITObject(&modelPoints[0], (int)modelPoints.size());

			//Estimate the pose
			cvPOSIT(positObject, &srcImagePoints[0], FOCAL_LENGTH, criteria, rotation_matrix, translation_vector);

			//Project axes
			std::vector<CvPoint2D32f> projectedAxesPoints;
			for (size_t p = 0; p<axesPoints.size(); p++)
			{
				CvPoint2D32f point2D = cvPoint2D32f(0.0, 0.0);
				transformAndProject(axesPoints[p], point2D, rotation_matrix, translation_vector);
				projectedAxesPoints.push_back(point2D);
			}

			//Draw the source image points
			for (size_t p = 0; p<modelPoints.size(); p++)
				circle(output, cvPoint((int)srcImagePoints[p].x, (int)srcImagePoints[p].y), 8, CV_RGB(255, 0, 0));


			//Draw the axes
			line(output, cvPoint((int)projectedAxesPoints[0].x, (int)projectedAxesPoints[0].y),
				cvPoint((int)projectedAxesPoints[1].x, (int)projectedAxesPoints[1].y), CV_RGB(0, 0, 255), 2);
			line(output, cvPoint((int)projectedAxesPoints[0].x, (int)projectedAxesPoints[0].y),
				cvPoint((int)projectedAxesPoints[2].x, (int)projectedAxesPoints[2].y), CV_RGB(255, 0, 0), 2);
			line(output, cvPoint((int)projectedAxesPoints[0].x, (int)projectedAxesPoints[0].y),
				cvPoint((int)projectedAxesPoints[3].x, (int)projectedAxesPoints[3].y), CV_RGB(0, 255, 0), 2);

			// Update look position on screen
			KF.predict();
			Mat_<float> measurement(2, 1); measurement.setTo(Scalar(0));
			measurement(0) = (projectedAxesPoints[3].x - projectedAxesPoints[0].x) / AXIS_SIZE;
			measurement(1) = (projectedAxesPoints[3].y - projectedAxesPoints[0].y) / AXIS_SIZE;
			Mat estimated = KF.correct(measurement);
			lookPosition[0] = estimated.at<float>(0);
			lookPosition[1] = estimated.at<float>(1);
		}

	}
}