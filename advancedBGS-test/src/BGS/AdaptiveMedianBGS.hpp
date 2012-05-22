/****************************************************************************
*
* AdaptiveMedianBGS.hpp
*
* Purpose: Implementation of the simple adaptive median background
*		  		 subtraction algorithm described in:
*	  			 "Segmentation and tracking of piglets in images"
* 						by McFarlane and Schofield
*
* Author: Donovan Parks, September 2007

Example:
		Algorithms::BackgroundSubtraction::AdaptiveMedianParams params;
		params.SetFrameSize(width, height);
		params.LowThreshold() = 40;
		params.HighThreshold() = 2*params.LowThreshold();
		params.SamplingRate() = 7;
		params.LearningFrames() = 30;

		Algorithms::BackgroundSubtraction::AdaptiveMedianBGS bgs;
		bgs.Initalize(params);
******************************************************************************/

#include "Bgs.hpp"

namespace Algorithms
{
namespace BackgroundSubtraction
{



// --- Adaptive Median BGS algorithm ---
class AdaptiveMedianBGS : public Bgs
{
public:
	virtual ~AdaptiveMedianBGS() {}

	void Initalize(const BgsParams& param);

	void InitModel(const RgbImage& data);
	void Subtract(int frame_num, const RgbImage& data,
									BwImage& low_threshold_mask, BwImage& high_threshold_mask);
	void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

    RgbImage* Background();


private:
	void SubtractPixel(int r, int c, const RgbPixel& pixel,
											unsigned char& low_threshold, unsigned char& high_threshold);
	BgsParams m_params;

	RgbImage m_median;
};

};
};
