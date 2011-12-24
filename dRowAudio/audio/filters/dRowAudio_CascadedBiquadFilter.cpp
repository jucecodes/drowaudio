/*
  ==============================================================================
  
  This file is part of the dRowAudio JUCE module
  Copyright 2004-12 by dRowAudio.
  
  ------------------------------------------------------------------------------
 
  dRowAudio can be redistributed and/or modified under the terms of the GNU General
  Public License (Version 2), as published by the Free Software Foundation.
  A copy of the license is included in the module distribution, or can be found
  online at www.gnu.org/licenses.
  
  dRowAudio is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  
  ==============================================================================
*/

#include "../../core/dRowAudio_StandardHeader.h"

BEGIN_DROWAUDIO_NAMESPACE

#include "dRowAudio_CascadedBiquadFilter.h"

CascadedBiquadFilter::CascadedBiquadFilter()
	:	currentFrequency(0.0)
{
	filterArray.add( new BiquadFilter() );
	currentOrder = filterArray.size();
}

CascadedBiquadFilter::~CascadedBiquadFilter()
{
	filterArray.clear();
}

void CascadedBiquadFilter::setOrder(int newOrder)
{
	if (newOrder != currentOrder)
	{
		// we dont want this to get too silly
		jlimit(1, 10, newOrder);
		
		if (newOrder > currentOrder)
		{
			for (int order = currentOrder; order < newOrder; order++) {
				BiquadFilter *newFilter = new BiquadFilter();
				newFilter->copyOutputsFrom( *filterArray[order-1] );
				newFilter->copyCoefficientsFrom( *filterArray[order-1] );
				filterArray.add(newFilter);
			}
		}
		else if (newOrder < currentOrder)
		{
			for (int order = currentOrder; order > newOrder; order--) {
				filterArray.removeLast();
			}
		}
		currentOrder = filterArray.size();
	}
}

void CascadedBiquadFilter::processSamples (float* const samples,
										   const int numSamples) throw()
{
	for (int i = 0; i < currentOrder; i++)	{
		filterArray.getUnchecked(i)->processSamples(samples, numSamples);
	}
}

float CascadedBiquadFilter::processSingleSampleRaw (const float sample) throw()
{
	float workingSample = sample;
	for (int i = 0; i < currentOrder; i++)	{
		workingSample = filterArray.getUnchecked(i)->processSingleSampleRaw(workingSample);
	}
	return workingSample;
}

void CascadedBiquadFilter::makeLowPass(const double sampleRate,
									   const double frequency) throw()
{
	if (frequency != currentFrequency)	{
		for (int i = 0; i < filterArray.size(); i++)
			filterArray.getUnchecked(i)->makeLowPass(sampleRate, frequency);
		currentFrequency = frequency;
	}
}

void CascadedBiquadFilter::makeHighPass(const double sampleRate,
										const double frequency) throw()
{
	if (frequency != currentFrequency)	{
		for (int i = 0; i < filterArray.size(); i++)
			filterArray.getUnchecked(i)->makeHighPass(sampleRate, frequency);
		currentFrequency = frequency;
	}
}

void CascadedBiquadFilter::makeBandPass(const double sampleRate,
										const double frequency,
										const double Q) throw()
{
	if (frequency != currentFrequency)	{
		for (int i = 0; i < filterArray.size(); i++)
			filterArray.getUnchecked(i)->makeBandPass(sampleRate, frequency, Q);
		currentFrequency = frequency;
	}
}

void CascadedBiquadFilter::makeBandStop(const double sampleRate,
										const double frequency,
										const double Q) throw()
{
	if (frequency != currentFrequency)	{
		for (int i = 0; i < filterArray.size(); i++)
			filterArray.getUnchecked(i)->makeBandStop(sampleRate, frequency, Q);
		currentFrequency = frequency;
	}
}

void CascadedBiquadFilter::makePeakNotch (const double sampleRate,
										  const double centreFrequency,
										  const double Q,
										  const float gainFactor) throw()
{
	if (centreFrequency != currentFrequency)	{
		for (int i = 0; i < filterArray.size(); i++)
			filterArray.getUnchecked(i)->makePeakNotch(sampleRate, centreFrequency, Q, gainFactor);
		currentFrequency = centreFrequency;
	}
}

void CascadedBiquadFilter::makeAllpass(const double sampleRate,
								 const double frequency,
								 const double Q) throw()
{
	if (frequency != currentFrequency)	{
		for (int i = 0; i < filterArray.size(); i++)
			filterArray.getUnchecked(i)->makeAllpass(sampleRate, frequency, Q);
		currentFrequency = frequency;
	}
}

END_DROWAUDIO_NAMESPACE