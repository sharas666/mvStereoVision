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

#include "utility.h"

class Subimage
{
  public:
    Subimage();
    Subimage(cv::Mat const&, int const&);
    ~Subimage();

    cv::Mat                 getSubMat() const;
    int                     getId() const;
    std::vector<Subimage>   getSubdividedImages() const;
    cv::Mat                 getSubdividedMatrix(int const&) const;

    void                    setSubMat(cv::Mat const&);
    void                    setId(int const&);

    std::pair<cv::Scalar, cv::Scalar> calcMeanStdDev() const;
    float                             calcMean() const;
    std::pair<float,float>            calcMinMax() const;
    void                              subdivide();

  private:
    int                   mId;
    cv::Mat               mSubimage;
    std::vector<Subimage> mSubdividedImages;
};

#endif //__SUBIMAGE_H__