#include "subimage.h"

Subimage::Subimage():
  mId(0),
  mSubimage(),
  mSubdividedImages()
{}

// final resolutions depending on binning are:
// with binning: 752x480 --> final subimage sizes: 250x160  200 pts per mSubMatrix  
// without binn: 376x240 --> final subimage sizes: 125x80   100 pts per mSubMatrix  13x8sp
Subimage::Subimage(cv::Mat const& mat, int const& id):
  mId(id),
  mSubimage(mat),
  mSubdividedImages()
{}


Subimage::~Subimage()
{
  mSubimage.release();
}


cv::Mat Subimage::getSubMat() const
{
  return mSubimage;
}

int Subimage::getId() const
{
  return mId; 
}

std::vector<Subimage> Subimage::getSubdividedImages() const
{
  return mSubdividedImages;
}

cv::Mat Subimage::getSubdividedMatrix(int const& index) const
{
  return mSubdividedImages[index].getSubMat();
}


void Subimage::setSubMat(cv::Mat const& matrix) 
{
  mSubimage = matrix;
}

void Subimage::setId(int const& id) 
{
  mId = id;
}

std::pair<cv::Scalar, cv::Scalar> Subimage::calcMeanStdDev() const 
{
  cv::Scalar mean, stdDev;
  cv::meanStdDev(mSubimage, mean, stdDev);

  auto returnValues = std::make_pair(mean,stdDev);
  return returnValues;
}

void Subimage::subdivide()
{
  int width = mSubimage.cols;
  int height = mSubimage.rows;

  cv::Rect tmpRect;
  cv::Mat tmpMat;
  int x1,x2,y1,y2;

  for (int i = 0; i < 9; ++i)
  {
    if(i < 3)
    {
      x1 = i%3*(width/3);
      x2 = (i%3+1)*(width/3);
      y1 = 0;
      y2 = height/3;
      
      tmpRect = cv::Rect(cv::Point(x1,y1),cv::Point(x2,y2));
      // std::cout << tmpRect << std::endl;
      tmpMat = mSubimage(tmpRect);

      mSubdividedImages.push_back(Subimage(tmpMat,i));
    }
    else if(i > 2 && i < 6)
    {
      x1 = i%3*(width/3);
      x2 = (i%3+1)*(width/3);
      y1 = height/3;
      y2 = height/3*2;

      tmpRect = cv::Rect(cv::Point(x1,y1),cv::Point(x2,y2));
      // std::cout << tmpRect << std::endl;
      tmpMat = mSubimage(tmpRect);

      mSubdividedImages.push_back(Subimage(tmpMat,i));
    }
    else if (i > 5)
    {
      x1 = i%3*(width/3);
      x2 = (i%3+1)*(width/3);
      y1 = height/3*2;
      y2 = height;

      tmpRect = cv::Rect(cv::Point(x1,y1),cv::Point(x2,y2));
      // std::cout << tmpRect << std::endl;
      tmpMat = mSubimage(tmpRect);

      mSubdividedImages.push_back(Subimage(tmpMat,i));
    }
  }

  tmpMat.release();
}