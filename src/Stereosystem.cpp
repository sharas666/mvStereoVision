#include "Stereosystem.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

Stereosystem::Stereosystem(Camera& l, Camera& r):
	mLeft(l),
	mRight(r),
	mR(),
	mE(),
	mT(),
	mF(),
	mIntrinsicLeft(),
	mIntrinsicRight(),
	mDistCoeffsLeft(),
	mDistCoeffsRight(),
	mTag("STEREOSYSTEM\t")
{
	std::cout<<"Stereosystem created\n";
}

Stereosystem::~Stereosystem()
{
	std::cout<<"Stereosystem destroyed\n";
}

double Stereosystem::calibrate(std::string path)
{
	std::cout<<"Calibration with path to images\n";
	return 0.0;
}


double Stereosystem::calibrate(std::vector<cv::Mat> const& leftImages, std::vector<cv::Mat> const& rightImages)
{
	std::cout<<"Calibration with given images\n";
	return 0.0;
}

bool Stereosystem::loadExtrinisic(std::string file)
{
 	cv::FileStorage fs;
 	bool success = fs.open(file, cv::FileStorage::READ);
  fs["R"] >> mR;
  fs["T"] >> mT;
  fs["E"] >> mE;
  fs["F"] >> mF;
  fs.release();

	return true;
}

bool Stereosystem::loadIntrinsic(std::string file, int cam)
{
	cv::FileStorage fs;
	bool success = fs.open(file, cv::FileStorage::READ);
  fs["cameraMatrixLeft"] >> mIntrinsicLeft;
  fs["cameraMatrixRight"] >> mIntrinsicLeft;
  fs["distCoeffsLeft"] >> mDistCoeffsLeft;
  fs["distCoeffsRight"] >> mDistCoeffsRight;
  fs.release();

	return success;
}

Stereopair Stereosystem::getImagepair()
{
	std::cout<<"Stereoimagepair requested\n";
	return Stereopair(cv::Mat(),cv::Mat());
}

Stereopair Stereosystem::getUndistortedImagpair()
{
	std::cout<<"Undistorted Stereoimagepair requested\n";
	return Stereopair(cv::Mat(),cv::Mat());
}

Stereopair Stereosystem::getRectifiedImagepair()
{
	std::cout<<"Rectified Stereoimagepair requested\n";
	return Stereopair(cv::Mat(),cv::Mat());
}