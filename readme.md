# FSEdit 

FSEdit is a Windows only Braille editor built on top of the RichEdit
control. It was originally used on the Freedom Scientific PAC Mate
note taker that ran along with JAWS under Windows CE, and the two were
tightly integrated. For the open source release, we took the code and
made just enough changes to get it to compile under Windows 10. Work
needs to be done to get it to actually be useful.

All JAWS specific
code has been removed. This primarily impacts forward and back
translation for which the interface remains but the implementation
does not. You'll need to change the LoadBrailleTranslator function to
return an object that implements IBrailleTranslator.

Spell checking functionality was originally implemented using Windows
CE specific APIs not available on desktop Windows. It will need to be
reimplemented using Hunspell or similar library.  For spell checking
Braille files, words are backtranslated one at a time and passed to
the spell checker.

## Code organization

FSEdit is the main executagle. It provides the main UI and hosts a
subclassed RichEdit window for editing Braille files. That subclassed
window is implemented in the BrlEdit project. It has a custom
WordBreakProc to allow using word navigation commands to move between
Braille words. Standard RichEdit
formatting commands can be used even when editing Braille files. If
those files are ultimately converted to RTF, the text is
backtranslated leaving the formatting alone. If they're saved as .brf
files, there's code that attempts to convert the RTF formatting
information into Braille symbols representing underline, italic,
bold, etc.  Conversion to and from RTF is done in the
ProjectConvertRtfAndBrltxt.

## Additional notes

When displaying Braille files, FSEdit uses the 7-bit characters stored
in the files themselves. This makes it hard for a screen reader
like JAWS to know if the text should be backtranslated when reading
using speech. It would be better to convert these characters to dot
patterns in the reserved Unicode range when streaming into the
RichEdit/BrlEdit  control. There's partial ssupport for this when
loading Braille files, but it's incomplete.
JAWS will be adding an option to enter Braille
directly as dot patterns in this special Unicode range, but this hasn't been done
yet.  

You'll also need to  change the WordBreakProc in BrlEdit to know
about characters in this range so that word navigation works properly.

