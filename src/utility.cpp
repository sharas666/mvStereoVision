#include "utility.h"


Stereopair::Stereopair(cv::Mat const& l , cv::Mat const& r):
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