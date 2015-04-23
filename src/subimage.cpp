#include "subimage.h"


Subimage::Subimage():
  mIndex(0),
  mSubMatrix(),
  mNumSamplepoints(0),
  mSamplepoints()
{}

Subimage::Subimage(cv::Mat const& mat, int const& index, int const& samplepoints):
  mIndex(index),
  mSubMatrix(mat),
  mNumSamplepoints(samplepoints)
{
  for(int i = 0; i < samplepoints; ++i)
  {

  }
}


Subimage::~Subimage()
{

}

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

int Subimage::getNumberOfSamplePoints() const
{
  return mNumSamplepoints;
}

// METHODS
void Subimage::displaySubImage() const
{
  std::cout << "draw call" << std::endl;
}