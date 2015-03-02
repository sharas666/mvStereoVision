#ifndef __Camera__H__
#define __Camera__H__

#include <vector>
#include <iostream>

#include <mvIMPACT_CPP/mvIMPACT_acquire.h>

#include "easylogging++.h"

class Camera
{
	public:
		Camera();
		Camera(mvIMPACT::acquire::Device*);
		~Camera();

		std::vector<char> getImage();

		void setExposure();
		void setGain();
		void setPixelFormat();
		
		//TODO getter for exposure, gain....
		//TODO setter for exposure, gain....

	private:
		mvIMPACT::acquire::Device* 				mDevice;
		mvIMPACT::acquire::FunctionInterface	mFunctionInterface;
		mvIMPACT::acquire::Request*				mRequest;
		int 									mTimeout;
		//TODO member
};



#endif