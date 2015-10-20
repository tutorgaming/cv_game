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
	HeadPose* mHeadPose;
public:

	CVProcess(void);
	virtual ~CVProcess(void);
	void init(void);

private:
	void runThread();

};