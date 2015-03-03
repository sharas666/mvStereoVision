#include "disparity.h"

void Disparity::sgbm(Stereopair const& inputImages, cv::Mat &output, cv::StereoSGBM dispCompute)
{
	dispCompute(inputImages.mLeft,inputImages.mRight,output);
	output/=16;
}