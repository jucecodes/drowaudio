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

BEGIN_JUCE_NAMESPACE

//========================================================================
FilteringAudioSource::FilteringAudioSource(AudioSource* inputSource,
                                           bool deleteInputWhenDeleted,
                                           int numChannels)
    : input         (inputSource, deleteInputWhenDeleted),
	  sampleRate    (44100.0),
	  lowEQGain     (1.0),
	  midEQGain     (1.0),
	  highEQGain    (1.0),
	  filterSource  (true)
{
	// instantiate the filters
	lowEQFilterL.makeLowShelf   (sampleRate, 70, 1.5, lowEQGain);
	lowEQFilterR.makeLowShelf   (sampleRate, 70, 1.5, lowEQGain);
	midEQFilterL.makeBandPass   (sampleRate, 1000, 1.5, midEQGain);
	midEQFilterR.makeBandPass   (sampleRate, 1000, 1.5, midEQGain);
	highEQFilterL.makeHighShelf (sampleRate, 1300, 1.5, highEQGain);
	highEQFilterR.makeHighShelf (sampleRate, 1300, 1.5, highEQGain);	
}

FilteringAudioSource::~FilteringAudioSource()
{
    releaseResources();
}

//==============================================================================
void FilteringAudioSource::setLowEQGain(float newLowEQGain)
{
	lowEQGain = newLowEQGain;

	lowEQFilterL.makeLowShelf (sampleRate, 500, 1, lowEQGain);
	lowEQFilterR.makeLowShelf (sampleRate, 500, 1, lowEQGain);
}

void FilteringAudioSource::setMidEQGain(float newMidEQGain)
{
	midEQGain = newMidEQGain;
	
	midEQFilterL.makeBandPass (sampleRate, 2000, 1, midEQGain);
	midEQFilterR.makeBandPass (sampleRate, 2000, 1, midEQGain);
}

void FilteringAudioSource::setHighEQGain (float newHighEQGain)
{
	highEQGain = newHighEQGain;
	
	highEQFilterL.makeHighShelf (sampleRate, 3500, 1, highEQGain);
	highEQFilterR.makeHighShelf(sampleRate, 3500, 1, highEQGain);	
}

void FilteringAudioSource::setFilterSource (bool shouldFilter)
{
	filterSource = shouldFilter;
}
//==============================================================================
void FilteringAudioSource::prepareToPlay (int samplesPerBlockExpected,
                                          double sampleRate_)
{
    const SpinLock::ScopedLockType sl (callbackLock);
	
    sampleRate = sampleRate_;
	
    lowEQFilterL.makeLowShelf   (sampleRate, 70, 1.5, lowEQGain);
	lowEQFilterR.makeLowShelf   (sampleRate, 70, 1.5, lowEQGain);
	midEQFilterL.makeBandPass   (sampleRate, 1000, 1.5, midEQGain);
	midEQFilterR.makeBandPass   (sampleRate, 1000, 1.5, midEQGain);
	highEQFilterL.makeHighShelf (sampleRate, 1300, 1.5, highEQGain);
	highEQFilterR.makeHighShelf (sampleRate, 1300, 1.5, highEQGain);	
    
    if (input != 0)
        input->prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void FilteringAudioSource::releaseResources()
{
    const SpinLock::ScopedLockType sl (callbackLock);
	
    if (input != 0)
        input->releaseResources();
}

void FilteringAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& info)
{
    const SpinLock::ScopedLockType sl (callbackLock);
		
    if (input != 0)
    {
        input->getNextAudioBlock (info);

		if (filterSource)
		{
			// filter samples
			const int bufferNumSamples = info.numSamples;
			float *sampleDataL = info.buffer->getSampleData (0, info.startSample);
			float *sampleDataR = info.buffer->getSampleData (1, info.startSample);
			
			lowEQFilterL.processSamples    (sampleDataL, bufferNumSamples);
			lowEQFilterR.processSamples    (sampleDataR, bufferNumSamples);
			midEQFilterL.processSamples    (sampleDataL, bufferNumSamples);
			midEQFilterR.processSamples    (sampleDataR, bufferNumSamples);
			highEQFilterL.processSamples   (sampleDataL, bufferNumSamples);
			highEQFilterR.processSamples   (sampleDataR, bufferNumSamples);
		}
    }
    else
    {
        info.clearActiveBufferRegion();
    }
}

END_JUCE_NAMESPACE