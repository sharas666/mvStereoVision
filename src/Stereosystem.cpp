#include "Stereosystem.h"

Stereosystem::Stereosystem(Camera& l, Camera& r):
	mLeft(l),
	mRight(r)
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
	std::cout<<"Load extrinsic from file\n";
	return true;
}

bool Stereosystem::loadIntrinsic(std::string file)
{
	std::cout<<"load intrinsic from file \n";
	return true;
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