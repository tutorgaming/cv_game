#include "HeadPose.hpp"

#define FOCAL_LENGTH 1000
#define CUBE_SIZE 10
#define AXIS_SIZE 100

HeadPose::HeadPose(void)
{
	
}

HeadPose::~HeadPose(void)
{
	delete rotation_matrix;
	delete translation_vector;
	cvReleasePOSITObject(&positObject);
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

void HeadPose::init(void)
{
	face_cascade.load("haarcascade_frontalface_default.xml");
	nose_cascade.load("Nariz_nuevo_20stages.xml");
	left_eye_cascade.load("haar_l_eye.xml");
	right_eye_cascade.load("haar_r_eye.xml");
	eye_cascade.load("haar_r_eye.xml");
	mouth_cascade.load("Mouth.xml");
	

	//Create the model points
	Point3f nosePoint(-0.0697709f, 18.6015f, 87.9695f);
	modelPoints.push_back(Point3f(0, 0, 0)); //nose (origin)
	modelPoints.push_back(Point3f(-36.9522f, 39.3518f, 47.1217f) - nosePoint);    //l eye
	modelPoints.push_back(Point3f(35.446f, 38.4345f, 47.6468f) - nosePoint);      //r eye
	modelPoints.push_back(Point3f(-27.6439f, -29.6388f, 73.8551f) - nosePoint);   //l mouth
	modelPoints.push_back(Point3f(28.7793f, -29.2935f, 72.7329f) - nosePoint);    //r mouth
	//modelPoints.push_back(Point3f(-87.2155f,15.5829f,-45.1352f) - nosePoint);   //l ear
	//modelPoints.push_back(Point3f(85.8383f,14.9023f,-46.3169f) - nosePoint);    //r ear

	// Create axes points
	axesPoints.push_back(Point3f(0, 0, 0));
	axesPoints.push_back(Point3f(AXIS_SIZE, 0, 0)); // X
	axesPoints.push_back(Point3f(0, AXIS_SIZE, 0)); // Y
	axesPoints.push_back(Point3f(0, 0, AXIS_SIZE)); // Z

	//Initialize etc
	rotation_matrix = new float[9];
	translation_vector = new float[3];
	criteria = cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 1.0e-4f);

}

void HeadPose::process(Mat &input, Mat &output)
{

	if (input.data) {
		output = input.clone();

		Mat gray;
		cvtColor(input, gray, COLOR_BGR2GRAY);

		vector<Rect> faces;
		face_cascade.detectMultiScale(gray, faces, 1.3, 6);

		for (size_t i = 0; i < faces.size(); i++) {
			Rect face_rect = faces[i];
			rectangle(output, faces[i], Scalar(0, 128, 0), 3); // Draw face roi

			Mat face_roi(gray, face_rect);
			Mat face_top_roi(face_roi, Rect(0, 0, face_rect.width, face_rect.height / 2));
			Mat face_bottom_roi(face_roi, Rect(0, face_rect.height*0.5, face_rect.width, face_rect.height*0.5));

			// Nose detection
			vector<Rect> noses;
			nose_cascade.detectMultiScale(face_roi, noses, 1.1, 20);


			for (size_t j = 0; j < noses.size() && j < 1; j++) {
				noses[j].x += face_rect.x;
				noses[j].y += face_rect.y;
				//cout << noses[i] << endl;

				rectangle(output, noses[j], Scalar(0, 128, 128), 3); // Draw noses roi
			}

			// Eye detection
			vector<Rect> eyes;
			eye_cascade.detectMultiScale(face_top_roi, eyes, 1.1, 20);

			for (size_t j = 0; j < eyes.size() && j < 2; j++) {
				eyes[j].x += face_rect.x;
				eyes[j].y += face_rect.y;


				rectangle(output, eyes[j], Scalar(128, 128, 128), 3); // Draw noses roi
			}

			// Mouth detection
			vector<Rect> mouths;
			mouth_cascade.detectMultiScale(face_bottom_roi, mouths, 1.3, 20);

			for (size_t j = 0; j < mouths.size() && j < 1; j++) {
				mouths[j].x += face_rect.x;
				mouths[j].y += face_rect.y + face_rect.width / 2;


				rectangle(output, mouths[j], Scalar(128, 128, 0), 3); // Draw noses roi
			}

			bool detectedAll = noses.size() >= 1 && eyes.size() >= 2 && mouths.size() >= 1;
			if (detectedAll) {
				// Update srcImage point
				srcImagePoints.clear();

				srcImagePoints.push_back(cvPoint2D32f(noses[0].x + noses[0].width / 2, noses[0].y + noses[0].height / 2)); // Move nose to top
				int leftEyeIdx = eyes[0].x < eyes[1].x ? 0 : 1;
				int rightEyeIdx = leftEyeIdx == 0 ? 1 : 0;
				srcImagePoints.push_back(cvPoint2D32f(eyes[leftEyeIdx].x + eyes[leftEyeIdx].width / 2, eyes[leftEyeIdx].y + eyes[leftEyeIdx].height / 2));
				srcImagePoints.push_back(cvPoint2D32f(eyes[rightEyeIdx].x + eyes[rightEyeIdx].width / 2, eyes[rightEyeIdx].y + eyes[rightEyeIdx].height / 2));
				srcImagePoints.push_back(cvPoint2D32f(mouths[0].x, mouths[0].y + mouths[0].height / 2));
				srcImagePoints.push_back(cvPoint2D32f(mouths[0].x + mouths[0].width, mouths[0].y + mouths[0].height / 2));

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

			}
		}
	}
}