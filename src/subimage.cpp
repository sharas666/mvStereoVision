#include "subimage.h"

Subimage::Subimage():
  mId("none"),
  mSubMat()
{}

// final resolutions depending on binning are:
// with binning: 752x480 --> final subimage sizes: 250x160  200 pts per mSubMatrix  
// without binn: 376x240 --> final subimage sizes: 125x80   100 pts per mSubMatrix  13x8sp
Subimage::Subimage(cv::Mat const& mat, std::string const& id):
  mId(id),
  mSubMat(mat)
{}


Subimage::~Subimage()
{
  mSubMat.release();
}


cv::Mat Subimage::getSubMat() const {
  return mSubMat;
}

std::string Subimage::getId() const {
  return mId; 
}


void Subimage::setSubMat(cv::Mat const& matrix) 
{
  mSubMat = matrix;
}

void Subimage::setId(std::string const& id) 
{
  mId = id;
}

std::pair<cv::Scalar, cv::Scalar> Subimage::calcMeanStdDev() const 
{
  cv::Scalar mean, stdDev;
  cv::meanStdDev(mSubMat, mean, stdDev);

  auto returnValues = std::make_pair(mean,stdDev);
  return returnValues;
}

void Subimage::subdivide(std::vector<Subimage>& returnVec)
{
  int width = mSubMat.rows;
  int height = mSubMat.cols;

  std::vector<Subimage> subdivisions;
  cv::Rect tmp;
  int x1,x2,y1,y2;

  for (int i = 0; i < 9; ++i)
  {
    if(i < 3)
    {
      x1 = i%3*(width/3);
      x2 = (i%3+1)*(width/3);
      y1 = 0;
      y2 = height/3;
      tmp = cv::Rect(cv::Point(x1,y1),cv::Point(x2,y2));
      std::cout << tmp << std::endl;
    }
    else if(i > 3 && i < 7)
    {
      x1 = i%3*(width/3);
      x2 = (i%3+1)*(width/3);
      y1 = height/3;
      y2 = height/3*2;
      tmp = cv::Rect(cv::Point(x1,y1),cv::Point(x2,y2));
      std::cout << tmp << std::endl;
    }
    else if (i > 7)
    {
      x1 = i%3*(width/3);
      x2 = (i%3+1)*(width/3);
      y1 = height/3*2;
      y2 = height;
      tmp = cv::Rect(cv::Point(x1,y1),cv::Point(x2,y2));
      std::cout << tmp << std::endl;
    }
  }
}