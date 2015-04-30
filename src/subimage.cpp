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

  std::pair<cv::Scalar, cv::Scalar> returnValues = std::pair<cv::Scalar, cv::Scalar>(mean,stdDev);
  return returnValues;

}