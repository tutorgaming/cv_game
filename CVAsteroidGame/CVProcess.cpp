#include "CVProcess.hpp"


//---------------------------------------------------------------------------
CVProcess::CVProcess(void)
{
	cap = cv::VideoCapture(0);
	cv::namedWindow("Webcam");
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
			//resize(captureFrame, captureFrame, cv::Size(captureFrame.cols / 2, captureFrame.rows / 2));
			flip(captureFrame, captureFrame, 1);
			mHeadPose->process(captureFrame, result);
			

			// Track hand
			Mat resultHand;
			Mat resizedImg;
			resize(captureFrame, resizedImg, cv::Size(captureFrame.cols / 4, captureFrame.rows / 4));
			mHandTracker->process(resizedImg, resultHand);
			Rect hRect = mHandTracker->getLastHandRect();
			rectangle(result, Rect(hRect.x * 4, hRect.y * 4, hRect.width * 4, hRect.height *4), Scalar(255, 0, 0));
			cv::imshow("Webcam", result);
		}

		// Wait
		//boost::this_thread::sleep(boost::posix_time::milliseconds(25));
	}
}

