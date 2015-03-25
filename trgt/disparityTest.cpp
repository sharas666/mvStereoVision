#include <iostream>
#include <string>

#include "Stereosystem.h"
#include "disparity.h"
#include "easylogging++.h"

#include <thread>
#include <mutex>
#include <condition_variable>

INITIALIZE_EASYLOGGINGPP


int main(int argc, char* argv[])
{
    std::string tag = "MAIN\t";

    LOG(INFO) << tag << "Application started.";

  std::vector<std::string> nodes;
  nodes.push_back("inputParameter");
  nodes.push_back("capturedDisparities");

  std::string config = "./configs/default.yml";

  cv::FileStorage fs;


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

    int key = 0;
    Stereopair s;

    cv::Mat left = cv::imread("./images/rectifiedImages/left/left_0010.jpg");
    cv::Mat right = cv::imread("./images/rectifiedImages/right/right_0010.jpg");

    cv::resize(left, left, cv::Size(), 0.5, 0.5, CV_INTER_AREA);
    cv::resize(right, right, cv::Size(), 0.5, 0.5, CV_INTER_AREA);

    cv::Mat grayLeft, grayRight;
    cv::cvtColor(left, grayLeft, CV_BGR2GRAY);
    cv::cvtColor(right, grayRight, CV_BGR2GRAY);
    s.mLeft = grayLeft;
    s.mRight = grayRight;

    int numDisp = 16;
    int windSize = 5;

    cv::StereoSGBM disparitySGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
    cv::StereoBM disparityBM(CV_STEREO_BM_BASIC, numDisp, windSize);


    cv::Mat normalizedSGBM;
    cv::Mat normalizedBM;
    cv::Mat normalizedTM;

    cv::Mat dispMapSGBM, dispMapBM, dispMapTM;

    bool running = true;

    while(running)
    {


        cv::imshow("Left", s.mLeft);
        cv::imshow("Right", s.mRight);

        Disparity::sgbm(s, dispMapSGBM, disparitySGBM);
        Disparity::bm(s, dispMapBM, disparityBM);
        Disparity::tm(s, dispMapTM,5);


        cv::normalize(dispMapSGBM,normalizedSGBM,0,255,cv::NORM_MINMAX, CV_8U);
        cv::imshow("SGBM",normalizedSGBM);

        cv::normalize(dispMapBM,normalizedBM,0,255,cv::NORM_MINMAX, CV_8U);
        cv::imshow("BM",normalizedBM);

        cv::normalize(dispMapTM,normalizedTM,0,255,cv::NORM_MINMAX, CV_8U);
        cv::imshow("TM",normalizedTM);

        key = cv::waitKey(10);


        if(char(key) == 'q')
        {
            running = false;
            break;
        }
        else if(char(key) == 'i')
        {
            numDisp +=16;
            disparitySGBM = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
            disparityBM = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);

        }
        else if(char(key) == 'd')
        {
            if( numDisp > 16)
            {
                numDisp -=16;
                disparitySGBM = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
                disparityBM = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);
            }

        }
        else if(char(key) == 'e')
        {
            windSize +=2;
            disparitySGBM = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
            disparityBM = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);
        }
        else if(char(key) == 'r')
        {
            if( windSize > 5)
            {
            windSize -=2;
            disparitySGBM = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
            disparityBM = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);
            }

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

                cv::normalize(dispMapSGBM,normalizedSGBM,0,255,cv::NORM_MINMAX, CV_8U);
                cv::imwrite(std::string(outputDirectory+"/sgbm_raw_"+prefix+std::to_string(imageNumber)+".jpg"),dispMapSGBM);
                cv::imwrite(std::string(outputDirectory+"/sgbm_norm_"+prefix+std::to_string(imageNumber)+".jpg"),normalizedSGBM);

                cv::normalize(dispMapBM,normalizedBM,0,255,cv::NORM_MINMAX, CV_8U);
                cv::imwrite(std::string(outputDirectory+"/bm_raw_"+prefix+std::to_string(imageNumber)+".jpg"),dispMapBM);
                cv::imwrite(std::string(outputDirectory+"/bm_norm_"+prefix+std::to_string(imageNumber)+".jpg"),normalizedBM);

                cv::normalize(dispMapTM,normalizedTM,0,255,cv::NORM_MINMAX, CV_8U);
                cv::imwrite(std::string(outputDirectory+"/tm_raw_"+prefix+std::to_string(imageNumber)+".jpg"),dispMapTM);
                cv::imwrite(std::string(outputDirectory+"/tm_norm_"+prefix+std::to_string(imageNumber)+".jpg"),normalizedTM);

            ++imageNumber;
        }
    }
    return 0;
}
