#ifndef __UTILITY__H__
#define __UTILITY__H__

#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

//OPENCV Stuff
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"

//Camera stuff
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>

//Logging stuff
#include "easylogging++.h"

#include "Camera.h"



struct Stereopair
{
	Stereopair();
	Stereopair(cv::Mat &, cv::Mat &);
	~Stereopair();


	cv::Mat mLeft;
	cv::Mat mRight;
	std::string mTag;

};

struct CameraInit
{
	Camera* left;
	Camera* right;
	bool init;
};


namespace Utility
{
	int getFiles (std::string const& dir, std::vector<std::string> &files);
	bool directoryExist(std::string const& dirPath);
	bool createDirectory(std::string const& dirPath);

	bool initCameras(mvIMPACT::acquire::DeviceManager&,Camera*&,Camera*&);

	bool checkConfig(std::string const&,std::vector<std::string> const&, cv::FileStorage &);
}


#endif

