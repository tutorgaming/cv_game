#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <opencv2/opencv.hpp>
#include "HeadPose.hpp"

//---------------------------------------------------------------------------

class CVProcess
{

private:
	boost::shared_ptr<boost::thread> mThread;
	cv::VideoCapture cap;
	cv::Mat captureFrame;
	bool mRunning;
	

public:
	HeadPose* mHeadPose;

private:
	void runThread();

public:

	CVProcess(void);
	virtual ~CVProcess(void);
	static CVProcess& getInstance()
	{
		static CVProcess instance;
		return instance;
	}
	void init(void);

};