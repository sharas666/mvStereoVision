#include "Camera.h"

Camera::Camera():
	mDevice(NULL),
	mFunctionInterface(NULL),
	mTimeout(1000),
	mTag(" CAMERA ")
	{}

Camera::Camera(mvIMPACT::acquire::Device* dev):
	mDevice(dev),
	mFunctionInterface(mvIMPACT::acquire::FunctionInterface(dev)),
	mTimeout(1000),
	mTag("CAMERA\t")
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
	std::cout<<"Image requested\n";

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
			return image;

		}
		mFunctionInterface.imageRequestUnlock(requestNr);
		std::cerr << "Error, request not OK!" <<std::endl;
	}

	std::cerr << "Error, invalid requestnumber!" << std::endl;
	return std::vector<char>(0,0);
}
