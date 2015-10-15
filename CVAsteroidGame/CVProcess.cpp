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
			cv::imshow("Webcam", captureFrame);
		}

		// Wait
		boost::this_thread::sleep(boost::posix_time::milliseconds(25));
	}
}