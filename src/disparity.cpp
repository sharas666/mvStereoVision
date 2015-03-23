#include "disparity.h"
#include "math.h"

void Disparity::sgbm(Stereopair const& inputImages, cv::Mat &output, cv::StereoSGBM dispCompute)
{
	dispCompute(inputImages.mLeft,inputImages.mRight,output);
	output/=16;
}

void Disparity::bm(Stereopair const& inputImages, cv::Mat &output, cv::StereoBM dispCompute)
{
  dispCompute(inputImages.mLeft,inputImages.mRight,output);
  output/=16;
}

void Disparity::ncc(Stereopair const& inputImages, cv::Mat &output)
{
  cv::Mat meanLeft, meanRight;
  cv::Mat devLeft, devRight;
  cv::Mat varianceLeft, varianceRight;

  cv::meanStdDev(inputImages.mLeft, meanLeft, meanLeft);
  cv::meanStdDev(inputImages.mRight, meanRight, meanRight);
  cv::pow(devLeft, 2, varianceLeft);
  cv::pow(devRight, 2, varianceRight);

  std::cout << meanLeft << std::endl;
  std::cout << meanRight << std::endl;
}