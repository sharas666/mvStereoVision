#ifndef __STEREOSYSTEM__H__
#define __STEREOSYSTEM__H__

#include <iostream>
#include <string>
#include <vector>

//OPENCV Stuff
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"


#include "Camera.h"
#include "utility.h"

class Stereosystem
{
	public:
		Stereosystem(Camera&, Camera&);
		~Stereosystem();

		double calibrate(std::string);
		double calibrate(std::vector<cv::Mat> const&, std::vector<cv::Mat> const&);

		bool loadExtrinisic(std::string);
		bool loadIntrinsic(std::string);

		Stereopair getImagepair();
		Stereopair getUndistortedImagpair();
		Stereopair getRectifiedImagepair();



	private:
		//TODO Memberstuff R,T,E,F......
		Camera 			mLeft;
		Camera 			mRight;

		std::string		mTag;
};

#endif