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

#include "dRowAudio_PositionableWaveDisplay.h"

PositionableWaveDisplay::PositionableWaveDisplay (AudioThumbnailImage& sourceToBeUsed)
    : currentSampleRate     (44100.0),
      audioThumbnailImage   (sourceToBeUsed),
      audioFilePlayer       (audioThumbnailImage.getAudioFilePlayer())
{
    audioThumbnailImage.addListener (this);
}

PositionableWaveDisplay::~PositionableWaveDisplay()
{
    audioThumbnailImage.removeListener (this);
	stopTimer (waveformUpdated);
}

//====================================================================================
void PositionableWaveDisplay::resized()
{
    cursorImage = Image (Image::RGB, 3, getHeight(), true);
    Graphics g (cursorImage);
    g.fillAll (Colours::black);
    g.setColour (Colours::white);
	g.drawVerticalLine (1, 0, cursorImage.getHeight());
    
    if (audioThumbnailImage.hasFinishedLoading()) 
    {
        cachedImage = audioThumbnailImage.getImage();
        cachedImage.duplicateIfShared();
        cachedImage = cachedImage.rescaled (getWidth(), getHeight());
    }
}

void PositionableWaveDisplay::paint(Graphics &g)
{
	const int w = getWidth();
	const int h = getHeight();

    g.setColour (Colours::black);	
    g.fillAll();

    Image thumbnail (cachedImage);
	g.drawImage (thumbnail,
                 0, 0, w, h,
                 0, 0, thumbnail.getWidth(), thumbnail.getHeight(), 
                 false);

    g.drawImageAt (cursorImage, transportLineXCoord.getCurrent() - 1, 0);
	
//	g.drawVerticalLine (transportLineXCoord.getCurrent() - 1, 0, h);
//	g.drawVerticalLine (transportLineXCoord.getCurrent() + 1, 0, h);
//
//	g.setColour (Colours::white);
//	g.drawVerticalLine (transportLineXCoord.getCurrent(), 0, h);
}

//====================================================================================
void PositionableWaveDisplay::timerCallback (const int timerId)
{
	if (timerId == waveformUpdated)
	{
		const int w = getWidth();
		const int h = getHeight();

		transportLineXCoord = w * oneOverFileLength * audioFilePlayer->getCurrentPosition();
		
		// if the line has moved repaint the old and new positions of it
		if (! transportLineXCoord.areEqual())
		{
			repaint (transportLineXCoord.getPrevious() - 2, 0, 5, h);
			repaint (transportLineXCoord.getCurrent() - 2, 0, 5, h);
		}
	}
}

//====================================================================================
void PositionableWaveDisplay::imageChanged (AudioThumbnailImage* changedAudioThumbnailImage)
{
	if (changedAudioThumbnailImage == &audioThumbnailImage)
	{
        cachedImage = Image::null;

		currentSampleRate = audioFilePlayer->getAudioFormatReaderSource()->getAudioFormatReader()->sampleRate;
		fileLength = audioFilePlayer->getLengthInSeconds();
		oneOverFileLength = 1.0 / fileLength;
		
		startTimer (waveformUpdated, 40);		
	}
}

void PositionableWaveDisplay::imageUpdated (AudioThumbnailImage* changedAudioThumbnailImage)
{
    cachedImage = audioThumbnailImage.getImage();
    repaint();
}

void PositionableWaveDisplay::imageFinished (AudioThumbnailImage* changedAudioThumbnailImage)
{
    resized();
}

//==============================================================================
void PositionableWaveDisplay::mouseDown (const MouseEvent &e)
{
	currentXScale = fileLength / getWidth();
	currentMouseX = e.x;
	
	setMouseCursor (MouseCursor::IBeamCursor);
	
	double position = currentXScale * currentMouseX;
	audioFilePlayer->setPosition (position);

    repaint();
}

void PositionableWaveDisplay::mouseUp (const MouseEvent &e)
{
	setMouseCursor (MouseCursor::NormalCursor);
}

void PositionableWaveDisplay::mouseDrag (const MouseEvent &e)
{
	currentMouseX = e.x;
	
	double position = currentXScale * currentMouseX;
	audioFilePlayer->setPosition (position);
}

END_JUCE_NAMESPACE