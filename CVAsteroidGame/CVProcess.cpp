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
			resize(captureFrame, captureFrame, cv::Size(captureFrame.cols / 2, captureFrame.rows / 2));
			flip(captureFrame, captureFrame, 1);
			mHeadPose->process(captureFrame, result);
			cv::imshow("Webcam", result);
		}

		// Wait
		//boost::this_thread::sleep(boost::posix_time::milliseconds(25));
	}
}

