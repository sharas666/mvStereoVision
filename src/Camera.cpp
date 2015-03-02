#include "Camera.h"

Camera::Camera():
	mDevice(NULL),
	mFunctionInterface(NULL),
	mStatistics(NULL),
  mSystemSettings(NULL),
  mCameraSettingsBase(NULL),
  mCameraSettingsBlueFOX(NULL),
  mImageDestinaton(NULL),
	mTimeout(1000),
	mTag(" CAMERA "),
	mWidth(0),
	mHeight(0)
	{}

Camera::Camera(mvIMPACT::acquire::Device* dev):
	mDevice(dev),
	mFunctionInterface(dev),
	mStatistics(dev),
  mSystemSettings(dev),
  mCameraSettingsBase(dev),
  mCameraSettingsBlueFOX(dev),
  mImageDestinaton(dev),
	mTimeout(1000),
	mTag("CAMERA\t"),
	mWidth(0),
	mHeight(0)
{
	std::cout<<"Camera created\n";
	//driver stuff
}

Camera::~Camera()
{
	std::cout<<"Camera destroyed\n";
	//clean up camera stuff
}


std::vector<char> Camera::getImage()
{
	//std::cout<<"Image requested\n";

	int result = DMR_NO_ERROR;
	//request an image
	result = mFunctionInterface.imageRequestSingle();

	if(result != DMR_NO_ERROR)
	{
		std::cerr << "Error while requesting for image: "<<\
		mvIMPACT::acquire::ImpactAcquireException::getErrorCodeAsString(result)<<\
		std::endl;
		LOG(ERROR)<< mTag << "Error while requesting for image: "<<\
		mvIMPACT::acquire::ImpactAcquireException::getErrorCodeAsString(result)<<\
		std::endl;
	}

	int requestNr = mFunctionInterface.imageRequestWaitFor(mTimeout);
	if(mFunctionInterface.isRequestNrValid(requestNr))
	{
		mRequest = mFunctionInterface.getRequest(requestNr);

		if(mRequest->isOK())
		{
			//create vector with image data from request
			std::vector<char> image(static_cast<char*>(mRequest->imageData.read()),
									static_cast<char*>(mRequest->imageData.read()) +\
															mRequest->imageSize.read());
			mWidth = mRequest->imageWidth.read();
			mHeight = mRequest->imageHeight.read();
			mFunctionInterface.imageRequestUnlock(requestNr);
			return image;

		}
		std::cerr << "Error, request not OK!" <<std::endl;

	}

	std::cerr << "Error, invalid requestnumber!" << std::endl;
	mFunctionInterface.imageRequestUnlock(requestNr);
	return std::vector<char>(0,0);
}

void Camera::setExposure(unsigned int exposure)
{
	mCameraSettingsBlueFOX.expose_us.write(exposure);
}

void Camera::setGain(float gain)
{
	mCameraSettingsBlueFOX.gain_dB.write(gain);
}

void Camera::setPixelFormat(int option)
{
	switch(option)
	{
		case 0: 
			mImageDestinaton.pixelFormat.write(mvIMPACT::acquire::idpfMono8);
			break;
		case 1: 
			mImageDestinaton.pixelFormat.write(mvIMPACT::acquire::idpfMono16);
			break;
	}
}

unsigned int Camera::getImageWidth()
{
	return mWidth;
}

unsigned int Camera::getImageHeight()
{
	return mHeight;
}
float Camera::getFramerate() const {
	return mStatistics.framesPerSecond.read();
}