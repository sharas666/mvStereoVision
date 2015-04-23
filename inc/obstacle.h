#ifndef __OBSTACLE__H__
#define __OBSTACLE__H__

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

struct Obstacle
{
  Obstacle();
  Obstacle(std::vector<float>, int);
  ~Obstacle();

  int getID() const;
  float getSize() const;
  float getMeanDistance() const;

  void setPolygon();
  
  cv::Rect    mArea;  
  int         mindex;
  std::string mTag;
};


#endif //__OBSTACLE__H__