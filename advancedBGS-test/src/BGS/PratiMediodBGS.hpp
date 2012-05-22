/****************************************************************************
*
* PratiMediodBGS.hpp
*
* Purpose: Implementation of the temporal median background 
*		  		 subtraction algorithm described in:
*
* [1] "Detecting Moving Objects, Shosts, and Shadows in Video Stream"
* 			by R. Cucchiara et al (2003)
*
* [2] "Reliable Background Suppression for Complex Scenes"
*				by S. Calderara et al (2006)
*
* Author: Donovan Parks, September 2007
*
* Please note that this is not an implementation of the complete system 
* given in the above papers. It simply implements the temporal media background
* subtraction algorithm.

Example:
		Algorithms::BackgroundSubtraction::PratiParams params;
		params.SetFrameSize(width, height);
		params.LowThreshold() = 30;
		params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
		params.SamplingRate() = 5;
		params.HistorySize() = 16;
		params.Weight() = 5;

		Algorithms::BackgroundSubtraction::PratiMediodBGS bgs;
		bgs.Initalize(params);
******************************************************************************/

#ifndef PRATI_MEDIA_BGS_H
#define PRATI_MEDIA_BGS_H

#include <vector>
#include "Bgs.hpp"

namespace Algorithms
{
namespace BackgroundSubtraction
{



// --- Prati Mediod BGS algorithm ---
class PratiMediodBGS : public Bgs 
{
private:	
	// sum of L-inf distances from a sample point to all other sample points
	struct MEDIAN_BUFFER
	{
		std::vector<RgbPixel> pixels;		// vector of pixels at give location in image
		std::vector<int> dist;					// distance from pixel to all other pixels
		int pos;												// current position in circular buffer

		RgbPixel median;								// median at this pixel location
		int medianDist;									// distance from median pixel to all other pixels
	};

public:
	PratiMediodBGS();
	~PratiMediodBGS();

	void Initalize(const BgsParams& param);

	void InitModel(const RgbImage& data);
	void Subtract(int frame_num, const RgbImage& data,  
									BwImage& low_threshold_mask, BwImage& high_threshold_mask);	
	void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

	RgbImage* Background() { return &m_background; }

private:	
	MEDIAN_BUFFER* m_median_buffer;

	void CalculateMasks(int r, int c, const RgbPixel& pixel);
	void Combine(const BwImage& low_mask, const BwImage& high_mask, BwImage& output);
	void UpdateMediod(int r, int c, const RgbImage& new_frame, int& dist);

	BgsParams m_params;
	
	RgbImage m_background;

	BwImage m_mask_low_threshold;
	BwImage m_mask_high_threshold;
};

};
};

#endif





