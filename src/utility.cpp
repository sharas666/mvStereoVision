#include "utility.h"

std::string mTag = "UTILITY\t";


Stereopair::Stereopair():
    mLeft(),
    mRight()
{}

Stereopair::Stereopair(cv::Mat &l , cv::Mat &r):
	mLeft(l),
	mRight(r)
{}


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



bool Utility::directoyExist(std::string const& dirPath)
{
	struct stat st = {0};
	if(stat(dirPath.c_str(),&st) == -1)
	{
		return false;
	}
	return true;
}

bool Utility::createDirectory(std::string const& dirPath)
{
	//split the path
	std::size_t pos = 2;
	
	while(pos < dirPath.length()-1)
	{
		pos = dirPath.find("/",pos+1);
		if(mkdir(dirPath.substr(0,pos).c_str(), 0777) == -1) 
  		{ 
			std::cerr << "Error("<< errno <<"): could not create: " << dirPath.substr(0,pos) << std::endl;
			return false;
  		} 
	}
  	return true;
}

int Utility::initCameras(mvIMPACT::acquire::DeviceManager &devMgr)
{
    const unsigned int devCnt = devMgr.deviceCount();

    if(devCnt != 2)
    {
        std::cerr << "Invalid numver of cameras detected! Number of detected cameras: " <<\
        devCnt << std::endl;
        LOG(ERROR)<< mTag <<"Invalid numver of cameras detected! Number of detected cameras: " <<\
        devCnt << std::endl;
        return -1;
    }

    mvIMPACT::acquire::Device* tmpDev_1 = devMgr[0];
    mvIMPACT::acquire::Device* tmpDev_2 = devMgr[1];

    if(tmpDev_1->serial.read() == "26803878")
    {
        if(tmpDev_2->serial.read() == "26803881")
        {
            std::cout<<"Successfully initilized both camers" <<std::endl;
            LOG(INFO)<< mTag << "Successfully initilized both camers" <<std::endl;
            return 0;
        }
        std::cerr << "Error in camera initialization, got Serials:" <<\
        tmpDev_1->serial.read()<< " " <<tmpDev_2->serial.read()<<std::endl;
        LOG(ERROR)<< mTag << "Error in camera initialization, got Serials:" <<\
        tmpDev_1->serial.read()<< " " <<tmpDev_2->serial.read()<<std::endl;
        return -1;
    }

    if(tmpDev_1->serial.read() == "26803881")
    {
        if(tmpDev_2->serial.read() == "26803878")
        {
            std::cout<<"Successfully initilized both camers" <<std::endl;
            LOG(INFO)<< mTag << "Successfully initilized both camers" <<std::endl;
            return 1;
        }
        std::cerr << "Error in camera initialization, got Serials:" <<\
        tmpDev_1->serial.read()<< " " <<tmpDev_2->serial.read()<<std::endl;
        LOG(ERROR)<< mTag << "Error in camera initialization, got Serials:" <<\
        tmpDev_1->serial.read()<< " " <<tmpDev_2->serial.read()<<std::endl;
        return -1;
    }

    return -1;
}
