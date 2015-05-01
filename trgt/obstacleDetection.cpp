#include <iostream>
#include <string>

//system command
#include <cstdlib>

#include "Stereosystem.h"
#include "disparity.h"
#include "easylogging++.h"
#include "subimage.h"

#include <thread>
#include <mutex>
#include <condition_variable>

INITIALIZE_EASYLOGGINGPP
bool running = true;

// threading stuff
std::mutex disparityLockSGBM;
std::condition_variable cond_var;

cv::StereoSGBM disparitySGBM;
int numDispSGBM = 64;
int windSizeSGBM = 9;
bool newDisparityMap = false;

cv::Mat dMapRaw;
cv::Mat dMapNorm;

cv::Mat Q, Q_32F;
cv::Mat R, R_32F;
cv::Mat disparityMap_32FC1;

void disparityCalc(Stereopair const& s, cv::StereoSGBM &disparity)
{
  while(running)
  {
    std::unique_lock<std::mutex> ul(disparityLockSGBM);
    cond_var.wait(ul);
    Disparity::sgbm(s, dMapRaw, disparity);
    dMapRaw.convertTo(disparityMap_32FC1,CV_32FC1);
    newDisparityMap=true;
  }
}

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
    std::cout << "x: " << x <<"  y: " << y << std::endl;
  }
}


void initWindows()
{
  cv::namedWindow("SGBM" ,1);
  cv::createTrackbar("Num Disp", "SGBM", &numDispSGBM, 320, changeNumDispSGBM);
  cv::createTrackbar("Wind Size", "SGBM", &windSizeSGBM, 51, changeWindSizeSGBM);
  cv::setMouseCallback("SGBM", mouseClick, NULL);
}

void drawObstacleGrid(cv::Mat &stream, int binning)
{
  cv::cvtColor(stream,stream,CV_GRAY2BGR);
  if(binning == 0) 
  {
    //vertical lines
    cv::line(stream, cv::Point(250,0), cv::Point(250,stream.rows), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(502,0), cv::Point(502,stream.rows), cv::Scalar(0,0,255), 1);
    //horizontal lines
    cv::line(stream, cv::Point(0,160), cv::Point(stream.cols, 160), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(0,320), cv::Point(stream.cols, 320), cv::Scalar(0,0,255), 1);
  }
  else
  {
    //vertical lines
    cv::line(stream, cv::Point(250/2,0), cv::Point(250/2,stream.rows), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(502/2,0), cv::Point(502/2,stream.rows), cv::Scalar(0,0,255), 1);
    //horizontal lines
    cv::line(stream, cv::Point(0,160/2), cv::Point(stream.cols, 160/2), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(0,320/2), cv::Point(stream.cols, 320/2), cv::Scalar(0,0,255), 1);
  }
}


int main(int argc, char* argv[])
{
  std::string tag = "MAIN\t";

  LOG(INFO) << tag << "Application started.";
  mvIMPACT::acquire::DeviceManager devMgr;

  Camera *left;
  Camera *right;

  if(!Utility::initCameras(devMgr,left,right))
  {
    return 0;
  }

  Stereosystem stereo(left,right);

  if(!stereo.loadIntrinsic("newCalibration/intrinsic.yml"))
  {
    return 0;
  }
  if(!stereo.loadExtrinisic("newCalibration/extrinsic.yml"))
  {
    return 0;
  }

  Stereopair s;
  left->setExposureMode(1);
  right->setExposureMode(1);

  char key = 0;
  int binning = 0;

  stereo.getRectifiedImagepair(s);
  Q = stereo.getQMatrix();
  Q.convertTo(Q_32F,CV_32F);

  cv::namedWindow("Left", cv::WINDOW_AUTOSIZE);
  cv::namedWindow("Right", cv::WINDOW_AUTOSIZE);
  initWindows();

  disparitySGBM = cv::StereoSGBM(0,numDispSGBM,windSizeSGBM,8*windSizeSGBM*windSizeSGBM,32*windSizeSGBM*windSizeSGBM);
  std::thread disparity(disparityCalc,std::ref(s),std::ref(disparitySGBM));

  cv::namedWindow("SubMat", cv::WINDOW_AUTOSIZE);

  bool running = true;
  int frame = 0;
  while(running)
  {

    stereo.getRectifiedImagepair(s);
    cv::imshow("Left", s.mLeft);
    cv::imshow("Right", s.mRight);

    if(newDisparityMap)
    {
      cv::normalize(dMapRaw,dMapNorm,0,255,cv::NORM_MINMAX, CV_8U);
      drawObstacleGrid(dMapNorm, binning);
      cv::imshow("SGBM",dMapNorm);
      newDisparityMap = false;
    }

    if (frame > 2) {
      Subimage sub = Subimage(dMapRaw, 0);
      sub.subdivide();
      std::vector<Subimage> subvec = sub.getSubdividedImages();
      for (unsigned int i = 0; i < subvec.size(); ++i)
      {
        subvec[i].subdivide();
      }

      // cv::Mat foo = subvec[4].getSubdividedMatrix(0);
      for (unsigned int i = 0; i < subvec.size(); ++i)
      {
        for (unsigned int j = 0; j < subvec.size(); ++j)
        {
          float mean = subvec[i].getSubdividedImages()[j].calcMeanStdDev().first[0];
          float distance = Utility::calcDistance(Q_32F, mean, binning);
          // std::cout << distance << std::endl;
        }
      }

      // cv::normalize(foo,fooNorm,0,255,cv::NORM_MINMAX, CV_8U);
      // cv::imshow("SubMat", fooNorm);
    }

    // notify the thread to start 
    cond_var.notify_one();
    key = cv::waitKey(5);

    // keypress stuff
    if(key > 0)
    {
      switch(key)
      {
        case 'q':
          LOG(INFO) << tag << "Exit requested" <<std::endl;
          delete left;
          left = nullptr;
          delete right;
          right = nullptr;
          cond_var.notify_one();
          running = false;
          break;
        case 'b':
          if (binning == 0)
            binning = 1;
          else
            binning =0;
          left->setBinning(binning);
          right->setBinning(binning);
          stereo.resetRectification();
          break;
        case 'f':
          std::cout<<left->getFramerate()<<" "<<right->getFramerate()<<std::endl;
          break;
        case 'd':
        {
          Subimage sub = Subimage(dMapRaw, 0);
          sub.subdivide();
          std::vector<Subimage> subvec = sub.getSubdividedImages();
          for (unsigned int i = 0; i < subvec.size(); ++i)
          {
            subvec[i].subdivide();
          }

          cv::Mat foo = subvec[4].getSubdividedMatrix(0);
          cv::Mat fooNorm;
          cv::normalize(foo,fooNorm,0,255,cv::NORM_MINMAX, CV_8U);
          cv::imshow("SubImageNorm", fooNorm);
          break;
        }
        case 't':
        {
          std::cout << Utility::calcDistance(Q_32F, 288, binning) << std::endl;
          std::cout << Utility::calcDistance(Q_32F, 341, binning) << std::endl;
          std::cout << Utility::calcDistance(Q_32F, 160, binning) << std::endl;
        }
        case 'e':
          std::cout << left->getExposure() << std::endl;
          std::cout << left->getExposure() << std::endl;
          break;
        default:
          std::cout << "Key pressed has no action" <<std::endl;
          break;
      }
    }
    ++frame;
  }

  disparity.join();

  return 0;
}
