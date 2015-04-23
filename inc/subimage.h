#ifndef __SUBIMAGE_H__
#define __SUBIMAGE_H__

#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

//OPENCV Stuff
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class Subimage
{
  public:
    Subimage();
    Subimage(cv::Mat const&, int const&, int const&);
    ~Subimage();

    void setSamplePoints(int const & );
    void setIndex(int);
    void setSubMatrix(cv::Mat const&);

    cv::Mat getSubMatrix() const;
    int getIndex() const;
    int getNumberOfSamplePoints() const;

    void displaySubImage() const;

  private:
    int                     mIndex;
    cv::Mat                 mSubMatrix;
    int                     mNumSamplepoints;
    std::vector<cv::Point>  mSamplepoints;
   
    std::string mTag;
};


#endif //__SUBIMAGE_H__