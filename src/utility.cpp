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
	//split the path
	// std::size_t pos = 2;
	
	// while(pos < dirPath.length()-1)
	// {
	// 	pos = dirPath.find("/",pos+1);
	// 	if(mkdir(dirPath.substr(0,pos).c_str(), 0777) == -1) 
 //  		{ 
	// 		LOG(ERROR)<< mTag << "Error("<< errno <<"): could not create: " << dirPath.substr(0,pos) << std::endl;
	// 		return false;
 //  		} 
	// }
 //  	return true;
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
        std::cerr << "Invalid numver of cameras detected! Number of detected cameras: " <<\
        devCnt << std::endl;
        LOG(ERROR)<< mTag <<"Invalid numver of cameras detected! Number of detected cameras: " <<\
        devCnt << std::endl;
        return false;
    }

   
    if(devMgr[0]->serial.read() == "26803878")
    {
        if(devMgr[1]->serial.read() == "26803881")
        {
            std::cout<<"Successfully initilized both camers" <<std::endl;
            LOG(INFO)<< mTag << "Successfully initilized both camers" <<std::endl;

            left = new Camera(devMgr[0]);
            right= new Camera(devMgr[1]);   
            return true;
        }
        std::cerr << "Error in camera initialization, got Serials:" <<\
        devMgr[0]->serial.read()<< " " <<devMgr[1]->serial.read()<<std::endl;
        LOG(ERROR)<< mTag << "Error in camera initialization, got Serials:" <<\
        devMgr[0]->serial.read()<< " " <<devMgr[1]->serial.read()<<std::endl;
        return false;
    }

    if(devMgr[0]->serial.read() == "26803881")
    {
        if(devMgr[1]->serial.read() == "26803878")
        {
            std::cout<<"Successfully initilized both camers" <<std::endl;
            LOG(INFO)<< mTag << "Successfully initilized both camers" <<std::endl;
            
            left = new Camera(devMgr[1]);
            right = new Camera(devMgr[0]);
            return true;
        }
        std::cerr << "Error in camera initialization, got Serials:" <<\
        devMgr[0]->serial.read()<< " " <<devMgr[1]->serial.read()<<std::endl;
        LOG(ERROR)<< mTag << "Error in camera initialization, got Serials:" <<\
        devMgr[0]->serial.read()<< " " <<devMgr[1]->serial.read()<<std::endl;
        return false;
    }

    return false;
}
