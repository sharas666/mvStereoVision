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
