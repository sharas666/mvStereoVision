#ifndef __SUBIMAGE_H__
#define __SUBIMAGE_H__

#include <vector>
#include <iostream>
#include <string>
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
    Subimage(cv::Mat const&, int const&);
    ~Subimage();

    cv::Mat getSubMat() const;
    int getId() const;

    void setSubMat(cv::Mat const&);
    void setId(int const&);

    std::pair<cv::Scalar, cv::Scalar> calcMeanStdDev() const;
    void subdivide();

  private:
    int                   mId;
    cv::Mat               mSubMat;
    std::vector<Subimage> mSubimages;
};

#endif //__SUBIMAGE_H__