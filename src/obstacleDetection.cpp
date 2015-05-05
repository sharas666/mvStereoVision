#include "obstacleDetection.h"
#include "utility.h"

obstacleDetection::obstacleDetection():
  mDispMap(),
  mBinning(0),
  mSamplepoints(),
  mSubimages(),
  mDistanceMapMean(),
  mDistanceMapMin(),
  mMeanMap()
{}

obstacleDetection::obstacleDetection(cv::Mat const& disparityMap, int binning):
  mDispMap(disparityMap),
  mBinning(binning),
  mSamplepoints(),
  mSubimages(),
  mDistanceMapMean(),
  mDistanceMapMin(),
  mMeanMap()
{
  //built the Subimage 'tree'
  Subimage sub = Subimage(mDispMap, 0);
  sub.subdivide();
  mSubimages = sub.getSubdividedImages();
  for (unsigned int i = 0; i < mSubimages.size(); ++i)
  {
    mSubimages[i].subdivide();
  }
}

obstacleDetection::~obstacleDetection()
{
  mDispMap.release();
}


std::vector<std::vector<float>> obstacleDetection::getMeanMap() const
{
  return mMeanMap;
}

std::vector<std::vector<float>> obstacleDetection::getDistanceMapMean() const
{
  return mDistanceMapMean;
}

std::vector<std::vector<float>> obstacleDetection::getDistanceMapMin() const
{
  return mDistanceMapMin;
}

std::vector<Subimage> obstacleDetection::getSubimages() const
{
  return mSubimages;
}


void obstacleDetection::buildMeanMap(cv::Mat const& Q)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> meanStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMean();
      meanStorage.push_back(mean);
    }
    mMeanMap.push_back(meanStorage);
    meanStorage.clear();
  }
}

void obstacleDetection::buildMeanDistanceMap(cv::Mat const& Q)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> distanceStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMean();
      float distance = Utility::calcDistance(Q, mean,mBinning);
      distanceStorage.push_back(distance);
    }
    mDistanceMapMean.push_back(distanceStorage);
    distanceStorage.clear();
  }
}


void obstacleDetection::buildMinDistanceMap(cv::Mat const& Q)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> distanceStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      //have to use the max value because of distance calculation
      float min = mSubimages[i].getSubdividedImages()[j].calcMinMax().second;
      float distance = Utility::calcDistance(Q, min, mBinning);
      distanceStorage.push_back(distance);
    }
    mDistanceMapMin.push_back(distanceStorage);
    distanceStorage.clear();
  }
}

void obstacleDetection::buildStdDevDistanceMap(cv::Mat const& Q)
{

}

void detectObstacles()
{

}
