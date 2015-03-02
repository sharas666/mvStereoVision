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
	mTag("CAMERA\tSerial:"+mDevice->serial.read() +\
		 " ID:" + std::to_string(mDevice->deviceID.read())+ "\t"),
	mWidth(0),
	mHeight(0)
{
	LOG(INFO)<< mTag <<"Camera created." << std::endl;

	//driver stuff
	this->setPixelFormat(MONO8);
	this->setExposure(12000);
	this->setGain(0);
}

Camera::~Camera()
{
	LOG(INFO)<< mTag <<"Camera destroyed." << std::endl;
	//clean up camera stuff
}


void Camera::getImage(std::vector<char> &imageToReturn)
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
			imageToReturn= std::vector<char>(static_cast<char*>(mRequest->imageData.read()),
									static_cast<char*>(mRequest->imageData.read()) +\
															mRequest->imageSize.read());
			mWidth = mRequest->imageWidth.read();
			mHeight = mRequest->imageHeight.read();
			mFunctionInterface.imageRequestUnlock(requestNr);
			return;

		}
		else
		{
			std::cerr << "Error, request not OK!" <<std::endl;
			LOG(ERROR) << mTag << "Error, request not OK!" <<std::endl;
			return;
		}
	}

	std::cerr << "Error, invalid requestnumber!" << std::endl;
	LOG(ERROR) << mTag << "Error, invalid requestnumber!" << std::endl;
	mFunctionInterface.imageRequestUnlock(requestNr);
}

void Camera::setExposure(unsigned int exposure)
{
	mCameraSettingsBlueFOX.expose_us.write(exposure);
	LOG(INFO) << mTag << "Set exposure time to " << exposure << std::endl;
}

void Camera::setGain(float gain)
{
	mCameraSettingsBlueFOX.gain_dB.write(gain);
	LOG(INFO) << mTag << "Set gain DB to " << gain << std::endl;
}

void Camera::setPixelFormat(int option)
{
	switch(option)
	{
		case MONO8: 
			mImageDestinaton.pixelFormat.write(mvIMPACT::acquire::idpfMono8);
			mCameraSettingsBase.pixelFormat.write(mvIMPACT::acquire::ibpfMono8);
			LOG(INFO) << mTag << "Set Pixelformat to Mono8" << std::endl;
			break;
		//doesnt work
		/*case MONO16: 
			mImageDestinaton.pixelFormat.write(mvIMPACT::acquire::idpfMono16);
			mCameraSettingsBase.pixelFormat.write(mvIMPACT::acquire::ibpfMono16);
			LOG(INFO) << mTag << "Set Pixelformat to Mono16" << std::endl;
			break;*/
	}
}

void Camera::setBinning(unsigned int option) 
{
	switch(option)
	{
		case BINNING_OFF:
			mCameraSettingsBlueFOX.binningMode.write(mvIMPACT::acquire::cbmOff);
			LOG(INFO) << mTag << "Set binning mode off." <<std::endl;
			break;
		case BINNING_V:
			mCameraSettingsBlueFOX.binningMode.write(mvIMPACT::acquire::cbmBinningV);
			LOG(INFO) << mTag << "Set vertical binning mode." <<std::endl;
			break;
		case BINNING_H:
			mCameraSettingsBlueFOX.binningMode.write(mvIMPACT::acquire::cbmBinningH);
			LOG(INFO) << mTag << "Set horizontal binning mode." <<std::endl;
			break;
		case BINNING_HV:
			mCameraSettingsBlueFOX.binningMode.write(mvIMPACT::acquire::cbmBinningHV);
			LOG(INFO) << mTag << "Set horizontal and vertical binning mode." <<std::endl;
			break;
		default:
			std::cerr << "Unknown binning mode: " << option << std::endl;
			LOG(WARNING)<< mTag << "Unknown binning mode: " << option <<\
								 ". No binning performed." << std::endl;
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

float Camera::getFramerate() const
{
	return mStatistics.framesPerSecond.read();
}

int Camera::getExposure() const
{
	return mCameraSettingsBlueFOX.expose_us.read();
}

float Camera::getGain() const
{
	return mCameraSettingsBlueFOX.gain_dB.read();
}