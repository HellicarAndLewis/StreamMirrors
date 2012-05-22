/****************************************************************************
*
* MeanBGS.hpp
*
* Purpose: Implementation of a simple temporal mean background 
*		  		 subtraction algorithm.
*
* Author: Donovan Parks, September 2007
*

Example:
		Algorithms::BackgroundSubtraction::MeanParams params;
		params.SetFrameSize(width, height);
		params.LowThreshold() = 3*30*30;
		params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
		params.Alpha() = 1e-6f;
		params.LearningFrames() = 30;

		Algorithms::BackgroundSubtraction::MeanBGS bgs;
		bgs.Initalize(params);
******************************************************************************/

#include "Bgs.hpp"

namespace Algorithms
{
namespace BackgroundSubtraction
{



// --- Mean BGS algorithm ---
class MeanBGS : public Bgs
{
public:
	virtual ~MeanBGS() {}

	void Initalize(const BgsParams& param);

	void InitModel(const RgbImage& data);
	void Subtract(int frame_num, const RgbImage& data,  
									BwImage& low_threshold_mask, BwImage& high_threshold_mask);	
	void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

	RgbImage* Background() { return &m_background; }

private:	
	void SubtractPixel(int r, int c, const RgbPixel& pixel, 
											unsigned char& lowThreshold, unsigned char& highThreshold);

	BgsParams m_params;

	RgbImageFloat m_mean;
	RgbImage m_background;
};

};
};





