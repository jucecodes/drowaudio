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
BasicFileBrowser::BasicFileBrowser (int flags_,
                                    const File& initialFileOrDirectory,
                                    const FileFilter* fileFilter_)
    : FileFilter (String::empty),
      fileFilter (fileFilter_),
      flags (flags_),
      thread ("Juce BasicFileBrowser"),
      showResizer(true)
{
    // You need to specify one or other of the open/save flags..
    jassert ((flags & (saveMode | openMode)) != 0);
    jassert ((flags & (saveMode | openMode)) != (saveMode | openMode));
    
    // You need to specify at least one of these flags..
    jassert ((flags & (canSelectFiles | canSelectDirectories)) != 0);
    
    String filename;
    
    if (initialFileOrDirectory == File::nonexistent)
    {
        currentRoot = File::getCurrentWorkingDirectory();
    }
    else if (initialFileOrDirectory.isDirectory())
    {
        currentRoot = initialFileOrDirectory;
    }
    else
    {
        chosenFiles.add (initialFileOrDirectory);
        currentRoot = initialFileOrDirectory.getParentDirectory();
        filename = initialFileOrDirectory.getFileName();
    }
    
    fileList = new DirectoryContentsList (this, thread);
	
    FileListComponent* const list = new FileListComponent (*fileList);
    fileListComponent = list;
    list->setOutlineThickness (0);
	list->getViewport()->setScrollBarThickness (10);
	list->setRowHeight (18);
    
    if ((flags & canSelectMultipleItems) != 0)
        list->setMultipleSelectionEnabled (true);
    
    addAndMakeVisible (list);
    	
    fileListComponent->addListener (this);
	list->getViewport()->getVerticalScrollBar()->setAutoHide (false);
		
	resizer = new ResizableCornerComponent (this, &resizeLimits);
	addAndMakeVisible (resizer);
	resizer->setMouseCursor (MouseCursor::LeftRightResizeCursor);

    setRoot (currentRoot);
		
    thread.startThread (4);
}

BasicFileBrowser::~BasicFileBrowser()
{
    fileListComponent = nullptr;
    fileList = nullptr;
	
    thread.stopThread (10000);
}

//==============================================================================
void BasicFileBrowser::addListener (FileBrowserListener* const newListener)
{
    listeners.add (newListener);
}

void BasicFileBrowser::removeListener (FileBrowserListener* const listener)
{
    listeners.remove (listener);
}

void BasicFileBrowser::mouseDoubleClick (const MouseEvent &e)
{
	if (resizer->contains (Point<int> (e.x, e.y)))
    {
		setSize (jmax (getLongestWidth(), resizeLimits.getMinimumWidth()), 800);
	}
}

//==============================================================================
bool BasicFileBrowser::isSaveMode() const noexcept
{
    return (flags & saveMode) != 0;
}

int BasicFileBrowser::getNumSelectedFiles() const noexcept
{
    if (chosenFiles.size() == 0 && currentFileIsValid())
        return 1;
    
    return chosenFiles.size();
}

File BasicFileBrowser::getSelectedFile (int index) const noexcept
{
    return chosenFiles[index];
}

bool BasicFileBrowser::currentFileIsValid() const
{
    if (isSaveMode())
        return ! getSelectedFile (0).isDirectory();
    else
        return getSelectedFile (0).exists();
}

File BasicFileBrowser::getHighlightedFile() const noexcept
{
    return fileListComponent->getSelectedFile (0);
}

void BasicFileBrowser::deselectAllFiles()
{
    fileListComponent->deselectAllFiles();
}

//==============================================================================
bool BasicFileBrowser::isFileSuitable (const File& file) const
{
    return (flags & canSelectFiles) != 0 && (fileFilter == nullptr || fileFilter->isFileSuitable (file));
}

bool BasicFileBrowser::isDirectorySuitable (const File&) const
{
    return true;
}

bool BasicFileBrowser::isFileOrDirSuitable (const File& f) const
{
    if (f.isDirectory())
        return (flags & canSelectDirectories) != 0
        && (fileFilter == nullptr || fileFilter->isDirectorySuitable (f));
    
    return (flags & canSelectFiles) != 0 && f.exists()
    && (fileFilter == nullptr || fileFilter->isFileSuitable (f));
}

//==============================================================================
const File& BasicFileBrowser::getRoot() const
{
    return currentRoot;
}

void BasicFileBrowser::setRoot (const File& newRootDirectory)
{
    bool callListeners = false;
    
    if (currentRoot != newRootDirectory)
    {
        callListeners = true;
        fileListComponent->scrollToTop();
    }
	
    currentRoot = newRootDirectory;
    fileList->setDirectory (currentRoot, true, true);
	
    if (callListeners)
    {
        Component::BailOutChecker checker (this);
        listeners.callChecked (checker, &FileBrowserListener::browserRootChanged, currentRoot);
    }    
}

void BasicFileBrowser::refresh()
{
    fileList->refresh();
}

void BasicFileBrowser::setFileFilter (const FileFilter* const newFileFilter)
{
    if (fileFilter != newFileFilter)
    {
        fileFilter = newFileFilter;
        refresh();
    }
}

String BasicFileBrowser::getActionVerb() const
{
    return isSaveMode() ? TRANS("Save") : TRANS("Open");
}

int BasicFileBrowser::getLongestWidth()
{
	const int noFiles = fileList->getNumFiles();
	int stringWidth = 0;
	
    FileListComponent* list = dynamic_cast<FileListComponent*> (fileListComponent.get());
    if (list != nullptr)
    {
        Font temp (list->getRowHeight() * 0.7f);
        for (int i = 0; i < noFiles; i++)
        {
            int itemWidth = temp.getStringWidth (fileList->getFile(i).getFileName());
            if (itemWidth >	stringWidth)
                stringWidth = itemWidth;
        }
        stringWidth += (2 * list->getRowHeight()) + 30;
    }
	
	return stringWidth;
}

DirectoryContentsDisplayComponent* BasicFileBrowser::getDisplayComponent() const noexcept
{
    return fileListComponent;
}

//==============================================================================
void BasicFileBrowser::resized()
{
	const int height = getHeight();
	const int width = getWidth();
		
    FileListComponent* list = dynamic_cast<FileListComponent*> (fileListComponent.get());

    if (list != nullptr)
    {
        list->setBounds (0, 0, width, height);
        ScrollBar *bar = list->getVerticalScrollBar();
        const int size = bar->getWidth() * 1.5;

        if (showResizer)
        {
            bar->setTransform (AffineTransform::scale (1, (height - (float) size) / height));
            
            resizeLimits.setSizeLimits (150, height, 1600, height);
            resizer->setBounds (width - size * (2.0 / 3.0), height - size, size * (2.0 / 3.0), size);
        }
        else 
        {
            bar->setTransform (AffineTransform::identity);
        }
    }
}

//==============================================================================
void BasicFileBrowser::sendListenerChangeMessage()
{
    Component::BailOutChecker checker (this);
    
    // You shouldn't delete the browser when the file gets changed!
    jassert (! checker.shouldBailOut());
    
    listeners.callChecked (checker, &FileBrowserListener::selectionChanged);
}

void BasicFileBrowser::selectionChanged()
{
    StringArray newFilenames;
    bool resetChosenFiles = true;

    for (int i = 0; i < fileListComponent->getNumSelectedFiles(); ++i)
    {
        const File f (fileListComponent->getSelectedFile (i));
        
        if (isFileOrDirSuitable (f))
        {
            if (resetChosenFiles)
            {
                chosenFiles.clear();
                resetChosenFiles = false;
            }
            
            chosenFiles.add (f);
            newFilenames.add (f.getRelativePathFrom (getRoot()));
        }
    }
    
    sendListenerChangeMessage();
}

void BasicFileBrowser::fileClicked (const File& f, const MouseEvent& e)
{
    Component::BailOutChecker checker (this);
    listeners.callChecked (checker, &FileBrowserListener::fileClicked, f, e);
}

void BasicFileBrowser::fileDoubleClicked (const File& f)
{
    if (f.isDirectory())
    {
        setRoot (f);
    }
    else
    {
        Component::BailOutChecker checker (this);
        listeners.callChecked (checker, &FileBrowserListener::fileDoubleClicked, f);
    }
}

void BasicFileBrowser::browserRootChanged (const File&) {}

bool BasicFileBrowser::keyPressed (const KeyPress& key)
{
#if JUCE_LINUX || JUCE_WINDOWS
    if (key.getModifiers().isCommandDown()
		&& (key.getKeyCode() == 'H' || key.getKeyCode() == 'h'))
    {
        fileList->setIgnoresHiddenFiles (! fileList->ignoresHiddenFiles());
        fileList->refresh();
        return true;
    }
#endif
	
    return false;
}

END_JUCE_NAMESPACE