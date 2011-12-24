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

#ifndef __REMOTEDIRECTORYLISTBOX_H_780D9778__
#define __REMOTEDIRECTORYLISTBOX_H_780D9778__

#include "../../JuceLibraryCode/JuceHeader.h"

class RemoteDirectoryListBoxModel : public ListBoxModel,
									public ChangeBroadcaster
{
public:
	
	RemoteDirectoryListBoxModel();
	
	~RemoteDirectoryListBoxModel();
	
	void setCURLSession(CURLEasySession *sessionToControl);
	
	void refresh();
	
	int getNumRows();

	void paintListBoxItem (int rowNumber,
						   Graphics& g,
                           int width, int height,
                           bool rowIsSelected);	
	
	void setContents(StringArray newContents);
	
	void listBoxItemDoubleClicked(int row, const MouseEvent &e);
	
	var getDragSourceDescription (const SparseSet<int> &currentlySelectedRows);

private:
	
	StringArray itemList;
	CURLEasySession* curlSession;
};


class RemoteDirectoryListBox :	public ListBox,
								public ChangeListener,
								public DragAndDropTarget
{
public:
	RemoteDirectoryListBox();
	
	~RemoteDirectoryListBox();

    void paintOverChildren (Graphics& g);

	CURLEasySession& getCURLSession()	{	return session;	}
	
	String getLastUrl()	{	return session.getCurrentWorkingDirectory();	}
	
	void refresh();
	
	void changeListenerCallback(ChangeBroadcaster* source);
	
	bool isInterestedInDragSource (const SourceDetails& dragSourceDetails);
	
    void itemDragEnter (const SourceDetails& dragSourceDetails);
    
    void itemDragExit (const SourceDetails& dragSourceDetails);
    
    void itemDropped (const SourceDetails& dragSourceDetails);
	
private:
	
	RemoteDirectoryListBoxModel model;
	CURLEasySession session;
    bool isInterestedInDrag;
};

#endif  // __REMOTEDIRECTORYLISTBOX_H_780D9778__
