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

//==============================================================================
struct MultipleThumbnailCacheEntry
{
    int64 hash;
    uint32 lastUsed;
    MemoryBlock data;

    JUCE_LEAK_DETECTOR (MultipleThumbnailCacheEntry);
};

//==============================================================================
MultipleAudioThumbnailCache::MultipleAudioThumbnailCache (const int maxNumThumbsToStore_)
    : TimeSliceThread ("thumb cache"),
      maxNumThumbsToStore (maxNumThumbsToStore_)
{
    startThread (2);
}

MultipleAudioThumbnailCache::~MultipleAudioThumbnailCache()
{
}

MultipleThumbnailCacheEntry* MultipleAudioThumbnailCache::findThumbFor (const int64 hash) const
{
    for (int i = thumbs.size(); --i >= 0;)
        if (thumbs.getUnchecked(i)->hash == hash)
            return thumbs.getUnchecked(i);

    return 0;
}

bool MultipleAudioThumbnailCache::loadThumb (FilteredAudioThumbnail& thumb, const int64 hashCode)
{
    MultipleThumbnailCacheEntry* te = findThumbFor (hashCode);
	
    if (te != 0)
    {
        te->lastUsed = Time::getMillisecondCounter();
		
        MemoryInputStream in (te->data, false);
        thumb.loadFrom (in);
        return true;
    }
	
    return false;
}

bool MultipleAudioThumbnailCache::loadThumb (ColouredAudioThumbnail& thumb, const int64 hashCode)
{
    MultipleThumbnailCacheEntry* te = findThumbFor (hashCode);

    if (te != 0)
    {
        te->lastUsed = Time::getMillisecondCounter();

        MemoryInputStream in (te->data, false);
        thumb.loadFrom (in);
        return true;
    }

    return false;
}

void MultipleAudioThumbnailCache::storeThumb (const ColouredAudioThumbnail& thumb,
											  const int64 hashCode)
{
    MultipleThumbnailCacheEntry* te = findThumbFor (hashCode);
	
    if (te == 0)
    {
        te = new MultipleThumbnailCacheEntry();
        te->hash = hashCode;
		
        if (thumbs.size() < maxNumThumbsToStore)
        {
            thumbs.add (te);
        }
        else
        {
            int oldest = 0;
            uint32 oldestTime = Time::getMillisecondCounter() + 1;
			
            for (int i = thumbs.size(); --i >= 0;)
            {
                if (thumbs.getUnchecked(i)->lastUsed < oldestTime)
                {
                    oldest = i;
                    oldestTime = thumbs.getUnchecked(i)->lastUsed;
                }
            }
			
            thumbs.set (oldest, te);
        }
    }
	
    te->lastUsed = Time::getMillisecondCounter();
	
    MemoryOutputStream out (te->data, false);
    thumb.saveTo (out);
}

void MultipleAudioThumbnailCache::storeThumb (const FilteredAudioThumbnail& thumb, int64 hashCode)
{
    MultipleThumbnailCacheEntry* te = findThumbFor (hashCode);
	
    if (te == 0)
    {
        te = new MultipleThumbnailCacheEntry();
        te->hash = hashCode;
		
        if (thumbs.size() < maxNumThumbsToStore)
        {
            thumbs.add (te);
        }
        else
        {
            int oldest = 0;
            uint32 oldestTime = Time::getMillisecondCounter() + 1;
			
            for (int i = thumbs.size(); --i >= 0;)
            {
                if (thumbs.getUnchecked(i)->lastUsed < oldestTime)
                {
                    oldest = i;
                    oldestTime = thumbs.getUnchecked(i)->lastUsed;
                }
            }
			
            thumbs.set (oldest, te);
        }
    }
	
    te->lastUsed = Time::getMillisecondCounter();
	
    MemoryOutputStream out (te->data, false);
    thumb.saveTo (out);
}

void MultipleAudioThumbnailCache::clear()
{
    thumbs.clear();
}

END_JUCE_NAMESPACE
