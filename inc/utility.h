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



struct Stereopair
{
	Stereopair(cv::Mat const&, cv::Mat const&);
	cv::Mat mLeft;
	cv::Mat mRight;
};


namespace Utility
{
	int getFiles (std::string const& dir, std::vector<std::string> &files);
	bool directoyExist(std::string const& dirPath);
	bool createDirectory(std::string const& dirPath);
}


#endif

