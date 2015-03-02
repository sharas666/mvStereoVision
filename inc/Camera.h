#ifndef __Camera__H__
#define __Camera__H__

#include <vector>
#include <iostream>
#include <string>

#include <mvIMPACT_CPP/mvIMPACT_acquire.h>

#include "easylogging++.h"

class Camera
{
	public:
		Camera();
		Camera(mvIMPACT::acquire::Device*);
		~Camera();

		void getImage(std::vector<char>&);

		void setExposure(unsigned int);
		void setGain(float);
		void setPixelFormat(int);

		float getFramerate() const;
		
		unsigned int getImageWidth();
		unsigned int getImageHeight();

		//TODO getter for exposure, gain....
		//TODO setter for exposure, gain....
		int getExposure() const;
		float getGain() const;

	private:
		mvIMPACT::acquire::Device* 								mDevice;
		mvIMPACT::acquire::FunctionInterface			mFunctionInterface;
	  mvIMPACT::acquire::Statistics 						mStatistics;
  	mvIMPACT::acquire::SystemSettings 				mSystemSettings;
  	mvIMPACT::acquire::CameraSettingsBase 		mCameraSettingsBase;
  	mvIMPACT::acquire::CameraSettingsBlueFOX 	mCameraSettingsBlueFOX;
  	mvIMPACT::acquire::ImageDestination 			mImageDestinaton;
		mvIMPACT::acquire::Request*								mRequest;
		int 																			mTimeout;
		std::string																mTag;



		unsigned int 							mWidth;
		unsigned int 							mHeight;

		//TODO member
};



#endif