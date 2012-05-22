/****************************************************************************
*
* ZivkovicAGMM.hpp
*
* Purpose: Implementation of the Gaussian mixture model (GMM) background 
*		  		 subtraction algorithm developed by Z. Zivkovic.
*
* Author: Donovan Parks, September 2007
*
* This code is based on code by Z. Zivkovic's. I have changed it from a pure
* C implementation to a cleaner (IMHO) C++ implementation. It is based on the
* following papers:
*
*	"Improved adaptive Gausian mixture model for background subtraction"
*		Z.Zivkovic 
*		International Conference Pattern Recognition, UK, August, 2004
*
*
* "Efficient Adaptive Density Estimapion per Image Pixel for the 
*			Task of Background Subtraction"
*		Z.Zivkovic, F. van der Heijden 
*		Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006.
*
* Zivkovic's code can be obtained at: www.zoranz.net

Example:
		Algorithms::BackgroundSubtraction::ZivkovicParams params;
		params.SetFrameSize(width, height);
		params.LowThreshold() = 5.0f*5.0f;
		params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
		params.Alpha() = 0.001f;
		params.MaxModes() = 3;

		Algorithms::BackgroundSubtraction::ZivkovicAGMM bgs;
		bgs.Initalize(params);
******************************************************************************/

#ifndef ZIVKOVIC_AGMM_H
#define ZIVKOVIC_AGMM_H

#include "Bgs.hpp"

namespace Algorithms
{
namespace BackgroundSubtraction
{



// --- Zivkovic AGMM BGS algorithm ---
class ZivkovicAGMM : public Bgs
{
private:
	struct GMM
	{
		float sigma;
		float muR;
		float muG;
		float muB;
		float weight;
	};

public:
	ZivkovicAGMM();
	~ZivkovicAGMM();

	void Initalize(const BgsParams& param);

	void InitModel(const RgbImage& data);
	void Subtract(int frame_num, const RgbImage& data,  
									BwImage& low_threshold_mask, BwImage& high_threshold_mask);	
	void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

	RgbImage* Background() { return &m_background; }

private:
	void SubtractPixel(long posPixel, const RgbPixel& pixel, unsigned char* pModesUsed, 
																	unsigned char& lowThreshold, unsigned char& highThreshold);
	
	// User adjustable parameters
	BgsParams m_params;

	// Threshold when the component becomes significant enough to be included into
	// the background model. It is the TB = 1-cf from the paper. So I use cf=0.1 => TB=0.9
	// For alpha=0.001 it means that the mode should exist for approximately 105 frames before
	// it is considered foreground
	float m_bg_threshold; //1-cf from the paper

	// Initial variance for the newly generated components. 
	// It will will influence the speed of adaptation. A good guess should be made. 
	// A simple way is to estimate the typical standard deviation from the images.
	float m_variance;

	// This is related to the number of samples needed to accept that a component
	// actually exists. 
	float m_complexity_prior;
	
	//data
	int m_num_bands;	//only RGB now ==3

	// dynamic array for the mixture of Gaussians
	GMM* m_modes;

	RgbImage m_background;

	//number of Gaussian components per pixel
	unsigned char* m_modes_per_pixel;
};

};
};

#endif





