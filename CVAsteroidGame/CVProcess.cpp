#include "CVProcess.hpp"


//---------------------------------------------------------------------------
CVProcess::CVProcess(void)
{
	cap = cv::VideoCapture(0);
}
//---------------------------------------------------------------------------
CVProcess::~CVProcess(void)
{
	mRunning = false;

	// Stops the thread
	assert(mThread);
	mThread->join();
}

void CVProcess::init(void)
{
	mRunning = true;

	// Create head pose
	mHeadPose = new HeadPose();
	mHeadPose->init();

	// Create hand tracker
	mHandTracker = new HandTracker();
	mHandTracker->init();

	// Create the thread and start work
	assert(!mThread);
	mThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CVProcess::runThread, this)));

	namedWindow("Webcam");
		
}

void CVProcess::runThread()
{
	// Process CV here!
	while (mRunning)
	{

		// Update data
		cap >> captureFrame;
		if (captureFrame.data) {
			cv::Mat result;
			flip(captureFrame, captureFrame, 1);
			// Track head
			mHeadPose->process(captureFrame, result);

			// Cut face off
			Mat imgNoFace = captureFrame.clone();			
			Rect lastFaceRect = mHeadPose->getLastFaceRect();
			Mat faceMat = imgNoFace.colRange(lastFaceRect.x, lastFaceRect.x + lastFaceRect.width).rowRange(lastFaceRect.y, imgNoFace.rows - 1);
			Mat zeroMask = Mat::zeros(faceMat.rows, faceMat.cols, CV_8UC3);
			zeroMask.copyTo(faceMat);

			// Track hand
			mHandTracker->process(imgNoFace);
			if(mHandTracker->isTracking())rectangle(result, mHandTracker->getLastHandRect(), Scalar(255,0 ,0));
			cv::imshow("Webcam", result);
		}

		// Wait
		//boost::this_thread::sleep(boost::posix_time::milliseconds(25));
	}
}

