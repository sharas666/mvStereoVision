#include "utility.h"

std::string mTag = "UTILITY\t";


Stereopair::Stereopair():
    mLeft(),
    mRight(),
    mTag("STEREOPAIR\t")
{}

Stereopair::Stereopair(cv::Mat &l , cv::Mat &r):
	mLeft(l),
	mRight(r),
    mTag("STEREOPAIR\t")

{}
Stereopair::~Stereopair()
{
    mLeft.release();
    mRight.release();
    LOG(INFO) << mTag << "Stereopair destroyed" << std::endl;
}


int Utility::getFiles (std::string const& dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;

    //Unable to open dir
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }

    //read files and push them to vector
    while ((dirp = readdir(dp)) != NULL)
    {
        std::string name = std::string(dirp->d_name);
        //discard . and .. from list
        if(name != "." && name != "..")
        {
            files.push_back(std::string(dirp->d_name));
        }
    }

    closedir(dp);
    std::sort(files.begin(), files.end());

    return 0;
}



bool Utility::directoryExist(std::string const& dirPath)
{
	struct stat st = {0};
	if(stat(dirPath.c_str(),&st) == 0)
	{
		return true;
	}
	return false;
}

bool Utility::createDirectory(std::string const& dirPath)
{
    system(std::string("mkdir -p " + dirPath).c_str());
    if(directoryExist(dirPath))
        return true;
    else
        return false;
}

bool Utility::initCameras(mvIMPACT::acquire::DeviceManager &devMgr, Camera *&left, Camera *&right)
{
    const unsigned int devCnt = devMgr.deviceCount();

    if(devCnt != 2)
    {
        LOG(ERROR)<< mTag <<"Invalid numver of cameras detected! Number of detected cameras: " <<\
        devCnt << std::endl;
        return false;
    }


    if(devMgr[0]->serial.read() == "26803878")
    {
        if(devMgr[1]->serial.read() == "26803881")
        {
            LOG(INFO)<< mTag << "Successfully initialized both camers" <<std::endl;

            left = new Camera(devMgr[0]);
            right= new Camera(devMgr[1]);
            return true;
        }
        LOG(ERROR)<< mTag << "Error in camera initialization, got Serials:" <<\
        devMgr[0]->serial.read()<< " " <<devMgr[1]->serial.read()<<std::endl;
        return false;
    }

    if(devMgr[0]->serial.read() == "26803881")
    {
        if(devMgr[1]->serial.read() == "26803878")
        {
            LOG(INFO)<< mTag << "Successfully initilized both camers" <<std::endl;

            left = new Camera(devMgr[1]);
            right = new Camera(devMgr[0]);
            return true;
        }
        LOG(ERROR)<< mTag << "Error in camera initialization, got Serials:" <<\
        devMgr[0]->serial.read()<< " " <<devMgr[1]->serial.read()<<std::endl;
        return false;
    }

    return false;
}


bool Utility::checkConfig(std::string const& configfile, std::vector<std::string> const& nodes, cv::FileStorage &fs)
{
    bool success = fs.open(configfile, cv::FileStorage::READ);

    for(std::string currentNode : nodes)
    {
        if(fs[currentNode].empty())
        {
            LOG(ERROR) << mTag << "Node " << currentNode << " in " << configfile << " is empty." << std::endl;
            fs.release();
            return false;
        }
    }

    if(success)
    {
        LOG(INFO) << mTag <<"Successfully checked configuration." << std::endl;;
        return true;
     }
    else
    {
        LOG(ERROR) << mTag << "Unable to open configuration: " << configfile << std::endl;
        fs.release();
        return false;
    }
}

bool Utility::calcLeftCoordinate(cv::Mat_<float> &toReturn, cv::Mat const& disparityMap,int x,int y,double baseline,double fx,double fy,double cx,double cy)
{
    double d = static_cast<float>(disparityMap.at<short>(y,x));
    d/=16.0;

    if(d > 0)
    {
        toReturn(2) = baseline*fx/d;
        toReturn(0) = toReturn(2)*(x-cx)/fx;
        toReturn(1) = toReturn(2)*(y-cy)/fy;
        return true;
    }
    else
    {
        LOG(WARNING) << mTag << "Disparity value not >= 0!" <<std::endl;
        return false;
    }
}


