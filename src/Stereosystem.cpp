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
	LOG(INFO)<< mTag <<"Stereosystem created\n";
}

Stereosystem::~Stereosystem()
{
	mR.release();
	mT.release();
	mE.release();
	mF.release();
	
	mMap1[0].release();
	mMap2[1].release();
	mR0.release();
	mR1.release(); 
	mP0.release(); 
	mP1.release(); 
	mQ.release();
	mIntrinsicLeft.release();
	mIntrinsicRight.release();
	mDistCoeffsLeft.release();
	mDistCoeffsRight.release();
	
	LOG(INFO)<< mTag <<"Stereosystem destroyed\n";
}

double Stereosystem::calibrate(std::string path)
{
	std::cout<<"Calibration with path to images\n";
	return 0.0;
}


double Stereosystem::calibrate(std::vector<cv::Mat> const& leftImages,
															 std::vector<cv::Mat> const& rightImages)

{

	if(leftImages.size() == rightImages.size())
	{
		mLeft->calibrate(leftImages);
		mRight->calibrate(rightImages);

		mIntrinsicLeft = mLeft->getIntrinsic();
		mIntrinsicRight = mRight->getIntrinsic();
		mDistCoeffsLeft = mLeft->getDistCoeffs();
		mDistCoeffsRight = mRight->getDistCoeffs();

		int hCorners = 9;
		int vCorners = 6;

		cv::Size boardSize = cv::Size(hCorners, vCorners);

		std::vector<std::vector<cv::Point3f> > objectPoints;
	  std::vector<std::vector<cv::Point2f> > imagePointsLeft;
	  std::vector<std::vector<cv::Point2f> > imagePointsRight;
	  std::vector<cv::Point2f> cornersLeft;
	  std::vector<cv::Point2f> cornersRight;
	  std::vector<cv::Point3f> obj;

	  // size of calibration patteren squares
	  float squaresize = 27.5;
	  for(int y=0; y<vCorners; ++y) {
	    for(int x=0; x<hCorners; ++x) {
	      obj.push_back(cv::Point3f((float(x)*squaresize),(float(y)*squaresize),0));
	    }
	  }

		for(unsigned int i = 0; i < leftImages.size(); ++i) {
			cv::Mat grayImageLeft;
			cv::Mat grayImageRight;

	  	cv::cvtColor(leftImages[i], grayImageLeft, CV_BGR2GRAY);
	  	cv::cvtColor(rightImages[i], grayImageRight, CV_BGR2GRAY);

	    bool foundL = cv::findChessboardCorners( grayImageLeft, boardSize, cornersLeft, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
	    bool foundR = cv::findChessboardCorners( grayImageRight, boardSize, cornersRight, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

			if(foundL && foundR) {
	      cv::cornerSubPix(grayImageLeft, cornersLeft, cv::Size(5,5), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 300, 0.1));
	      cv::cornerSubPix(grayImageRight, cornersRight, cv::Size(5,5), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 300, 0.1));

	      imagePointsLeft.push_back(cornersLeft);
	      imagePointsRight.push_back(cornersRight);

	      objectPoints.push_back(obj);

	      std::cout << "Found " << i << std::endl;

	      grayImageLeft.release();
	      grayImageRight.release();
			}
			else
			{
				std::cout << "Unable to find Corners in image " << i << ". Image ignored" << std::endl;
				continue;
			}
	  }

	  cv::Size imagesize = cv::Size(leftImages[0].size());
		double stereoRMS = cv::stereoCalibrate(objectPoints,imagePointsLeft,imagePointsRight,
                                           mIntrinsicLeft,mDistCoeffsLeft,mIntrinsicRight,mDistCoeffsRight,
                                           imagesize,mR,mT,mE,mF,
                                           cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 300, 1e-7));

		return stereoRMS;
	}

	return -1.0;

}

bool Stereosystem::loadExtrinisic(std::string const& file )
{
	cv::FileStorage fs;
 	bool success = fs.open(file, cv::FileStorage::READ);
  	fs["R"] >> mR;
  	fs["T"] >> mT;
  	fs["E"] >> mE;
  	fs["F"] >> mF;
  	fs.release();
  	LOG(INFO) << mTag <<"loaded Extrinsics.\n";
	return success;
}

bool Stereosystem::loadIntrinsic(std::string const& file)
{
	cv::FileStorage fs;
	bool success = fs.open(file, cv::FileStorage::READ);
  	fs["cameraMatrixLeft"] >> mIntrinsicLeft;
  	fs["cameraMatrixRight"] >> mIntrinsicRight;
  	fs["distCoeffsLeft"] >> mDistCoeffsLeft;
  	fs["distCoeffsRight"] >> mDistCoeffsRight;
  	fs.release();
  	LOG(INFO) << mTag <<"loaded Intrinsics.\n";
	return success;
}

bool Stereosystem::saveExtrinsic(std::string const& file)
{
	cv::FileStorage fs;
 	bool success = fs.open(file, cv::FileStorage::WRITE);
  	fs << "R" << mR;
  	fs << "T" << mT;
  	fs << "E" << mE;
  	fs << "F" << mF;
  	fs.release();
  	LOG(INFO) << mTag <<"saved Extrinsics.\n";
	return success;
}

bool Stereosystem::saveIntrinsic(std::string const& file)
{
	cv::FileStorage fs;
	bool success = fs.open(file, cv::FileStorage::WRITE);
  	fs << "cameraMatrixLeft" << mIntrinsicLeft;
  	fs << "cameraMatrixRight" << mIntrinsicRight;
  	fs << "distCoeffsLeft" << mDistCoeffsLeft;
  	fs << "distCoeffsRight" << mDistCoeffsRight;
  	fs.release();
  	LOG(INFO) << mTag <<"saved Intrinsics.\n";
	return success;
}

bool Stereosystem::getImagepair(Stereopair& stereoimagepair)
{
	std::vector<char> leftImage;
	//mLeft->getImage(leftImage);
	std::vector<char> rightImage;
	//mRight->getImage(rightImage);
	std::future<bool> l = std::async(&Camera::getImage,mLeft,std::ref(leftImage));
	std::future<bool> r = std::async(&Camera::getImage,mRight,std::ref(rightImage));

	if(l.get() && r.get())
	{
		cv::Mat(mLeft->getImageHeight(),mLeft->getImageWidth(), CV_8UC1, &leftImage[0]).copyTo(stereoimagepair.mLeft);
		cv::Mat(mRight->getImageHeight(),mRight->getImageWidth(), CV_8UC1, &rightImage[0]).copyTo(stereoimagepair.mRight);	
		return true;
	}
	return false;
}

bool Stereosystem::getUndistortedImagepair(Stereopair& sip)
{
	cv::Mat tmpLeft, tmpRight;
	if(this->getImagepair(sip))
	{
		cv::undistort(sip.mLeft, tmpLeft, mIntrinsicLeft, mDistCoeffsLeft);
		cv::undistort(sip.mRight, tmpRight, mIntrinsicRight, mDistCoeffsRight);
		tmpLeft.copyTo(sip.mLeft);
		tmpRight.copyTo(sip.mRight);
		return true;
	}

	return false;
}

bool Stereosystem::initRectification()
{
	LOG(INFO) << mTag << "Called initRectifiaction()" << std::endl;

	cv::Size imagesizeL(mLeft->getImageWidth(), mLeft->getImageHeight());
	cv::Size imagesizeR(mRight->getImageWidth(), mRight->getImageHeight());
	LOG(INFO) << mTag << "Imagessizes are: "<< imagesizeL << " "<< imagesizeR <<std::endl;
	
	if(imagesizeL == imagesizeR)
	{
		if(mLeft->getBinningMode() && mRight->getBinningMode())
		{
			mIntrinsicLeft /=2;
			mIntrinsicRight/=2;
		}

		cv::stereoRectify(mIntrinsicLeft, mDistCoeffsLeft, mIntrinsicRight, mDistCoeffsRight,
		                      imagesizeL, mR, mT, mR0, mR1, mP0, mP1, mQ, CV_CALIB_ZERO_DISPARITY, 0, 
		                      imagesizeL, &mValidROI[0], &mValidROI[1]);
		
		cv::initUndistortRectifyMap(mIntrinsicLeft, mDistCoeffsLeft, mR0, mP0, imagesizeL, CV_32FC1, mMap1[0], mMap2[0]);
		cv::initUndistortRectifyMap(mIntrinsicRight, mDistCoeffsRight, mR1, mP1, imagesizeL, CV_32FC1, mMap1[1], mMap2[1]);

		mDisplayROI = mValidROI[0] & mValidROI[1];
		
		LOG(INFO) << mTag << "Rectification successfully initialized!" <<std::endl;
		mIsInit = true;

		if(mLeft->getBinningMode() && mRight->getBinningMode())
		{
			mIntrinsicLeft *=2;
			mIntrinsicRight*=2;
		}
  		return true;
	}
	else
	{
		LOG(ERROR) << mTag << "Unable to init rectification" <<std::endl;
		return false;
	}	
	return false;
}

bool Stereosystem::getRectifiedImagepair(Stereopair& sip)
{

	this->getImagepair(sip);
	if(mIsInit)
	{
		cv::remap(sip.mLeft, sip.mLeft, mMap1[0], mMap2[0], cv::INTER_LINEAR);
    cv::remap(sip.mRight, sip.mRight, mMap1[1], mMap2[1], cv::INTER_LINEAR);

   	sip.mLeft = sip.mLeft(mDisplayROI);
   	sip.mLeft = sip.mLeft(mDisplayROI);
   	return true;

	}
	else
	{
		if(!this->initRectification())
		{
			LOG(ERROR) << "rectification failed!\n";
			return false;
		}
		else
		{
			cv::remap(sip.mLeft, sip.mLeft, mMap1[0], mMap2[0], cv::INTER_LINEAR);
	    	cv::remap(sip.mRight, sip.mRight, mMap1[1], mMap2[1], cv::INTER_LINEAR);

	   		sip.mLeft = sip.mLeft(mDisplayROI);
	   		sip.mLeft = sip.mLeft(mDisplayROI);
	   		return true;
		
		}
		return false;
	}
	return false;
 }

 void Stereosystem::resetRectification()
 {
 	mIsInit = false;
 }

 void Stereosystem::getFundamentalMatrix(cv::Mat &fundamental) const
 {
 	mF.copyTo(fundamental);
 }

 void Stereosystem::getTranslationMatrix(cv::Mat &translation) const
 {
 	mT.copyTo(translation);
 }