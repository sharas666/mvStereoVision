#include <iostream>
#include <string>

#include "Stereosystem.h"
#include "disparity.h"
#include "easylogging++.h"

#include <thread>
#include <mutex>
#include <condition_variable>

INITIALIZE_EASYLOGGINGPP

bool running = true;
cv::Mat dispMapSGBM, dispMapBM, dispMapNCC;

cv::StereoSGBM disparitySGBM;

int numDispSGBM = 16;
int windSizeSGBM = 5;

double baseline ;
double fx ;
double fy ;
double cx ;
double cy ;

cv::Mat R, R_32F;

void changeNumDispSGBM(int, void*)
{
  numDispSGBM+=numDispSGBM%16;

  if(numDispSGBM < 16)
  {
    numDispSGBM = 16;
    cv::setTrackbarPos("Num Disp", "SGBM", numDispSGBM);
  }

  cv::setTrackbarPos("Num Disp", "SGBM", numDispSGBM);
  disparitySGBM = cv::StereoSGBM(0,numDispSGBM,windSizeSGBM,8*windSizeSGBM*windSizeSGBM,32*windSizeSGBM*windSizeSGBM);
}

void changeWindSizeSGBM(int, void*)
{
  if(windSizeSGBM%2 == 0)
    windSizeSGBM+=1;

  if(windSizeSGBM < 5)
  {
    windSizeSGBM = 5;
    cv::setTrackbarPos("Wind Size", "SGBM", windSizeSGBM);
  }
  cv::setTrackbarPos("Wind Size", "SGBM", windSizeSGBM);
  disparitySGBM = cv::StereoSGBM(0,numDispSGBM,windSizeSGBM,8*windSizeSGBM*windSizeSGBM,32*windSizeSGBM*windSizeSGBM);
}

void mouseClick(int event, int x, int y,int flags, void* userdata)
{

  if  ( event == CV_EVENT_LBUTTONDOWN )
   {

      cv::Mat_<float>  coordinate(3,1);
      if(Utility::calcLeftCoordinate(coordinate,dispMapSGBM,x,y,baseline,fx,fy,cx,cy))
      {
        coordinate = R_32F.t() * coordinate;
        std::cout<<"distance: " << (sqrt(pow(coordinate(0),2)+pow(coordinate(1),2)+pow(coordinate(2),2))/1000.0) <<std::endl;
      }
      else
      {
        std::cout <<"invalid disparity\n";
      }

   }
   // else if  ( event == CV_EVENT_RBUTTONDOWN )
   // {
   //      std::cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
   // }
   // else if  ( event == CV_EVENT_MBUTTONDOWN )
   // {
   //      std::cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
   // }
   // else if ( event == CV_EVENT_MOUSEMOVE )
   // {
   //      std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;

   // }


}


void initWindows()
{
  cv::namedWindow("SGBM" ,1);
  //cv::namedWindow("BM" ,1);

  cv::createTrackbar("Num Disp", "SGBM", &numDispSGBM, 320, changeNumDispSGBM);
  cv::createTrackbar("Wind Size", "SGBM", &windSizeSGBM, 51, changeWindSizeSGBM);

  // cv::createTrackbar("Num Disp", "BM", &numDispBM, 320, changeNumDispBM);
  // cv::createTrackbar("Wind Size", "BM", &windSizeBM, 51, changeWindSizeBM);

  cv::setMouseCallback("SGBM", mouseClick, NULL);

}


int main(int argc, char* argv[])
{
  std::string tag = "MAIN\t";

  LOG(INFO) << tag << "Application started.";
  mvIMPACT::acquire::DeviceManager devMgr;

  Camera *left;
  Camera *right;

  initWindows();

  if(!Utility::initCameras(devMgr,left,right))
  {
    return 0;
  }


  std::vector<std::string> nodes;
  nodes.push_back("inputParameter");
  nodes.push_back("capturedDisparities");

  std::string config = "./configs/default.yml";

  cv::FileStorage fs;
  left->setExposure(20000);
  right->setExposure(20000);

  if(!Utility::checkConfig(config,nodes,fs))
  {
  return 0;
  }

  std::string inputParameter;
  fs["inputParameter"] >> inputParameter;

  std::string outputDirectory;
  fs["capturedDisparities"] >> outputDirectory;

  int imageNumber = 0;

  if(Utility::directoryExist(outputDirectory))
  {
    std::vector<std::string> tmp1;
    Utility::getFiles(outputDirectory,tmp1);

    if(tmp1.size() != 0)
    {
      std::cout << "Output directory not empty, clean files? [y/n] " <<std::endl;
      char key = getchar();
      if(key == 'y')
      {
        std::system(std::string("rm " + outputDirectory + "/* -f ").c_str());
      }
      else if(key == 'n')
      {
        imageNumber = tmp1.size();
        LOG(INFO) << tag << "Start with image number " << imageNumber << std::endl;
      }
    }
  }

  if(Utility::createDirectory(outputDirectory))
  {
    LOG(INFO) << tag << "Successfully created directory for captured disparity maps." << std::endl;
  }
  else
  {
    LOG(ERROR) << tag << "Unable to create directoryfor captured disparity maps." <<std::endl;
    return 0;
  }

  Stereosystem stereo(left,right);

  if(!stereo.loadIntrinsic(inputParameter+"/intrinsic.yml"))
  return 0;
  if(!stereo.loadExtrinisic(inputParameter +"/extrinsic.yml"))
  return 0;

  baseline =stereo.getBaseline();
  fx = left->getIntrinsic().at<double>(0,0);
  fy = left->getIntrinsic().at<double>(1,1);
  cx = left->getIntrinsic().at<double>(0,2);
  cy = left->getIntrinsic().at<double>(1,2);

  R = stereo.getRotationMatrix();

  R.convertTo(R_32F,CV_32F);
  int key = 0;
  int binning = 0;
  Stereopair s;

  disparitySGBM = cv::StereoSGBM(0,numDispSGBM,windSizeSGBM,8*windSizeSGBM*windSizeSGBM,32*windSizeSGBM*windSizeSGBM);
  //disparityBM  = cv::StereoBM(CV_STEREO_BM_BASIC, numDispBM, windSizeBM);

  cv::Mat normalizedSGBM;
  //cv::Mat normalizedBM;
  while(running)
  {
    stereo.getRectifiedImagepair(s);
    cv::imshow("Left", s.mLeft);
    cv::imshow("Right", s.mRight);

    Disparity::sgbm(s, dispMapSGBM, disparitySGBM);
    cv::normalize(dispMapSGBM,normalizedSGBM,0,255,cv::NORM_MINMAX, CV_8U);
    cv::imshow("SGBM",normalizedSGBM);

    key = cv::waitKey(10);

    if(char(key) == 'q')
    {
      running = false;
    }
    else if(char(key) == 'b')
    {
      if (binning == 0)
      {
        binning = 1;
        fx = left->getIntrinsic().at<double>(0,0)/2.0;
        fy = left->getIntrinsic().at<double>(1,1)/2.0;
        cx = left->getIntrinsic().at<double>(0,2)/2.0;
        cy = left->getIntrinsic().at<double>(1,2)/2.0;
      }
      else
      {
        binning =0;
        fx = left->getIntrinsic().at<double>(0,0);
        fy = left->getIntrinsic().at<double>(1,1);
        cx = left->getIntrinsic().at<double>(0,2);
        cy = left->getIntrinsic().at<double>(1,2);
      }

      left->setBinning(binning);
      right->setBinning(binning);
      stereo.resetRectification();
    }
    else if (char(key) == 'c')
    {
      std::string prefix = "";
      if(imageNumber < 10)
      {
        prefix +="000";
      }
      else if ((imageNumber >=10) && (imageNumber <100))
      {
        prefix += "00";
      }
      else if(imageNumber >= 100)
      {
        prefix +="0";
      }

      {
        cv::normalize(dispMapSGBM,normalizedSGBM,0,255,cv::NORM_MINMAX, CV_8U);
        cv::imwrite(std::string(outputDirectory+"/sgbm_raw_"+prefix+std::to_string(imageNumber)+".jpg"),dispMapSGBM);
        cv::imwrite(std::string(outputDirectory+"/sgbm_norm_"+prefix+std::to_string(imageNumber)+".jpg"),normalizedSGBM);
      }
      ++imageNumber;
    }
    else if(char(key) == 'f')
      std::cout<<left->getFramerate()<<" "<<right->getFramerate()<<std::endl;

  }
  return 0;
}
