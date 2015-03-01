#include "Camera.h"

Camera::Camera():
	mDevice(NULL),
	mFunctionInterface(NULL),
	mTimeout(1000)
	{}

Camera::Camera(mvIMPACT::acquire::Device* dev):
	mDevice(dev),
	mFunctionInterface(NULL),
	mTimeout(1000)
{
	std::cout<<"Camera created\n";
	//driver stuff
	mFunctionInterface = FunctionInterface(dev);
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
		CLOG(ERROR,"camera") << "Error while requesting for image: "<<\
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
			std::vector<char> image(reinterpret_cast<char*>(mRequest->imageData.read()),
									reinterpret_cast<char*>(mRequest->imageData.read()) +\
															mRequest->imageSize.read());
			return image;

		}
		mFunctionInterface.imageRequestUnlock(requestNr);
		std::cerr << "Error, request not OK!" <<std::endl;
	}

	std::cerr << "Error, invalid requestnumber!" << std::endl;
	return std::vector<char>(0,0);
}
