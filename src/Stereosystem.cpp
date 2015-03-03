#include "Stereosystem.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

Stereosystem::Stereosystem(Camera* l, Camera* r):
	mLeft(l),
	mRight(r),
	mR(),
	mT(),
	mE(),
	mF(),
	mIsInit(false),
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

	return success;
}

bool Stereosystem::loadIntrinsic(std::string file)
{
	cv::FileStorage fs;
	bool success = fs.open(file, cv::FileStorage::READ);
  fs["cameraMatrixLeft"] >> mIntrinsicLeft;
  fs["cameraMatrixRight"] >> mIntrinsicRight;
  fs["distCoeffsLeft"] >> mDistCoeffsLeft;
  fs["distCoeffsRight"] >> mDistCoeffsRight;
  fs.release();

	return success;
}

void Stereosystem::getImagepair(Stereopair& stereoimagepair)
{
	std::vector<char> leftImage;
	//mLeft->getImage(leftImage);
	std::vector<char> rightImage;
	//mRight->getImage(rightImage);
	std::thread l(&Camera::getImage,mLeft,std::ref(leftImage));
	std::thread r(&Camera::getImage,mRight,std::ref(rightImage));
	l.join();
	r.join();
	cv::Mat(mLeft->getImageHeight(),mLeft->getImageWidth(), CV_8UC1, &leftImage[0]).copyTo(stereoimagepair.mLeft);
	cv::Mat(mRight->getImageHeight(),mRight->getImageWidth(), CV_8UC1, &rightImage[0]).copyTo(stereoimagepair.mRight);
}

void Stereosystem::getUndistortedImagepair(Stereopair& sip)
{
	this->getImagepair(sip);

	cv::undistort(sip.mLeft, sip.mLeft, mIntrinsicLeft, mDistCoeffsLeft);
	cv::undistort(sip.mRight, sip.mRight, mIntrinsicRight, mDistCoeffsRight);
}

bool Stereosystem::initRectification()
{

	std::cout << "foo" << std::endl;

	cv::Size imagesizeL(mLeft->getImageWidth(), mLeft->getImageHeight());
	cv::Size imagesizeR(mRight->getImageWidth(), mRight->getImageHeight());

	cv::stereoRectify(mIntrinsicLeft, mDistCoeffsLeft, mIntrinsicRight, mDistCoeffsRight,
                      imagesizeL, mR, mT, mR0, mR1, mP0, mP1, mQ, CV_CALIB_ZERO_DISPARITY, 0, 
                      imagesizeL, &mValidROI[0], &mValidROI[1]);

	cv::initUndistortRectifyMap(mIntrinsicLeft, mDistCoeffsLeft, mR0, mP0, imagesizeL, CV_32FC1, mMap1[0], mMap2[0]);
  cv::initUndistortRectifyMap(mIntrinsicRight, mDistCoeffsRight, mR1, mP1, imagesizeL, CV_32FC1, mMap1[1], mMap2[1]);

  mDisplayROI = mValidROI[0] & mValidROI[1];

  mIsInit = true;
  return true;
}

void Stereosystem::getRectifiedImagepair(Stereopair& sip)
{

	this->getImagepair(sip);

	if(mIsInit)
	{
		cv::remap(sip.mLeft, sip.mLeft, mMap1[0], mMap2[0], cv::INTER_LINEAR);
    cv::remap(sip.mRight, sip.mRight, mMap1[1], mMap2[1], cv::INTER_LINEAR);

    sip.mLeft = sip.mLeft(mDisplayROI);
    sip.mLeft = sip.mLeft(mDisplayROI);
	} else
	{
		if(!this->initRectification())
			LOG(ERROR) << "rectification failed!\n";
		else
			this->getRectifiedImagepair(sip);
	}
 }