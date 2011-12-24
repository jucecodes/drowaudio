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

#ifndef __DROWAUDIO_COMPARATORS_H__
#define __DROWAUDIO_COMPARATORS_H__

namespace ValueTreeComparators {

class Lexicographic
{
public:
    Lexicographic (const Identifier attributeToSort_, bool forwards)
    :	attributeToSort (attributeToSort_),
        direction (forwards ? 1 : -1)
    {
    }
    
    int compareElements (const ValueTree &first, const ValueTree &second) const
    {
        int result = 0;
        
        result = first[attributeToSort].toString().compareLexicographically(second[attributeToSort].toString());
        
        return direction * result;
    }
    
private:
    const Identifier attributeToSort;
    const int direction;
};

class Numerical
{
public:
    Numerical (const Identifier attributeToSort_, bool forwards)
    :	attributeToSort (attributeToSort_),
        direction (forwards ? 1 : -1)
    {
    }
    
    int compareElements (const ValueTree &first, const ValueTree &second) const
    {
        int result = 0;
        
        result = (double(first[attributeToSort]) > double(second[attributeToSort])) ? 1 : -1;
        
        return direction * result;
    }
    
private:
    const Identifier attributeToSort;
    const int direction;
};	
	
class LexicographicWithBackup
{
public:
    LexicographicWithBackup (const Identifier attributeToSort_, const Identifier backupAttribute_, bool forwards)
        : attributeToSort (attributeToSort_),
          backupAttribute (backupAttribute_),
          direction (forwards ? 1 : -1)
    {
    }
    
    int compareElements (const ValueTree &first, const ValueTree &second) const
    {
        int result = 0;
        
        result = first[attributeToSort].toString().compareLexicographically (second[attributeToSort].toString());
        
        if (result == 0)
            result = first[backupAttribute].toString().compareLexicographically (second[backupAttribute].toString());
        
        return direction * result;
    }
    
private:
    const Identifier attributeToSort;
    const Identifier backupAttribute;
    const int direction;
};
    
} //ValueTreeComparators

namespace XmlComparators {

//==============================================================================
// A comparator used to sort our data when the user clicks a column header
class LexicographicSorter
{
public:
	LexicographicSorter (const String attributeToSort_, bool forwards)
		: attributeToSort (attributeToSort_),
		  direction (forwards ? 1 : -1)
	{
	}

	int compareElements (XmlElement* first, XmlElement* second) const
	{
		int result = first->getStringAttribute (attributeToSort)
					   .compareLexicographically (second->getStringAttribute (attributeToSort));

		if (result == 0)
			result = first->getStringAttribute ("ID")
					   .compareLexicographically (second->getStringAttribute ("ID"));

		return direction * result;
	}

private:
	const String attributeToSort;
	const int direction;
};

//==============================================================================
// A comparator used to sort number data
class NumberDataSorter
{
public:
	NumberDataSorter (const String attributeToSort_, bool forwards)
	: attributeToSort (attributeToSort_),
	  direction (forwards ? 1 : -1)
	{
	}
	
	int compareElements (XmlElement* first, XmlElement* second) const
	{
		if (first->getStringAttribute (attributeToSort).isEmpty()
			|| second->getStringAttribute (attributeToSort).isEmpty()) {
			return direction * -1;
		}
		
		int result = ((first->getStringAttribute (attributeToSort).getDoubleValue() 
					   <= (second->getStringAttribute (attributeToSort).getDoubleValue()))
					  ? -1 : 1);
//            if (result == 0)
//                result = first->getStringAttribute (T("ID"))
//				.compareLexicographically (second->getStringAttribute (T("ID")));
		
		return direction * result;
	}
	
private:
	const String attributeToSort;
	const int direction;
};

} //XmlComparators

#endif  // __DROWAUDIO_COMPARATORS_H__
