/****************************************************************************
*
* BgsParams.hpp
*
* Purpose: Base class for BGS parameters. Any parameters common to all BGS
*					 algorithms should be specified directly in this class.
*
* Author: Donovan Parks, May 2008
*
******************************************************************************/

#ifndef BGS_PARAMS_H_
#define BGS_PARAMS_H_

namespace Algorithms
{
namespace BackgroundSubtraction
{

class BgsParams
{
public:
	virtual ~BgsParams() {}

	virtual void SetFrameSize(unsigned int width, unsigned int height)
	{
		m_width = width;
		m_height = height;
		m_size = width*height;
	}

	unsigned int &Width() { return m_width; }
	unsigned int &Height() { return m_height; }
	unsigned int &Size() { return m_size; }

	float &LowThreshold() { return m_low_threshold; }
	float &HighThreshold() { return m_high_threshold; }
	
	int &HistorySize() { return m_history_size; }
	int &EmbeddedDim() { return m_dim; }
	int &SamplingRate() { return m_samplingRate; }
	int &LearningFrames() { return m_learning_frames; }

	float &Alpha() { return m_alpha; }
	int &MaxModes() { return m_max_modes; }
	int &Weight() { return m_weight; }
	
protected:

	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_size;

	int m_max_modes;
	float m_alpha;
	
	// A pixel will be classified as foreground if the squared distance of any
	// color channel is greater than the specified threshold
	float m_low_threshold;
	float m_high_threshold;
	
	int m_dim;	
	// The weight parameter controls the amount of influence given to previous background samples
	// see w_b in equation (2) of [1]
	// in [2] this value is set to 1
	int m_weight;
	
	int m_history_size;
	
	// Rate at which to obtain new samples
	int m_sampling_rate;
	

	int m_samplingRate;
	int m_learning_frames;
	
};

};
};

#endif