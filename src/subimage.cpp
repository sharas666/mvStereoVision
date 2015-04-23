#include "subimage.h"


Subimage::Subimage():
  mIndex(0),
  mSubMatrix(),
  mNumSamplepoints(0),
  mSamplepoints()
{}

// final resolutions depending on binning are:
// with binning: 752x480 --> final subimage sizes: 250x160  200 pts per mSubMatrix  
// without binn: 376x240 --> final subimage sizes: 125x80   100 pts per mSubMatrix  13x8sp
Subimage::Subimage(cv::Mat const& mat, int const& index, int const& samplepoints):
  mIndex(index),
  mSubMatrix(mat),
  mNumSamplepoints(samplepoints)
{
  std::vector<int> xCoords, yCoords;
  for (int i = 0; i < samplepoints; ++i)
  {
    
  }
}


Subimage::~Subimage()
{}

// SETTER
void Subimage::setSamplePoints(int const& numSP)
{
  std::cout << "setter called!" << std::endl;
}

void Subimage::setIndex(int index)
{
  mIndex = index;
}

void Subimage::setSubMatrix(cv::Mat const& newSubMat)
{
  mSubMatrix = newSubMat;
}

// GETTER
cv::Mat Subimage::getSubMatrix() const
{
  return mSubMatrix;
}

int Subimage::getIndex() const
{
  return mIndex;
}

int Subimage::getNumberOfSamplepoints() const
{
  return mNumSamplepoints;
}

std::vector<cv::Point> Subimage::getSamplepoints() const
{
  return mSamplepoints;
}
