How to compile RTIViewer
-------------------------------------------------------------------------------------

To compile RTIViewer you need a C++ compiling environment and the following libraries:
- Qt 4.7 (http://www.qtsoftware.com/downloads).
- the VCG libraries; this library is only accessible through anonymous svn (svn string:
  svn co https://vcg.svn.sourceforge.net/svnroot/vcg vcg ). The library must be at the
  same level of the folder 'components' and its root should be named 'vcglib'

The code tree must have the following structure:
~/components/RTIViewer/
~/vcglib/vcg/space/
 
The compiling step depends on the compiling environment. Using GCC (both under linux and
using the mingw gcc provided with the free Qt distribution) you should just type, from
the '~/components/RTIViewer/' directory:

qmake -recursive rtiviewerv10.pro
make

If you want to use Visual Studio, you can install the integration of the Qt tools into the
Visual Studio IDE (http://qt.nokia.com/downloads/visual-studio-add-in) and import the top
level pro ( ~/components/RTIViewer/rtiviewerv10.pro ) into VisualStudio. Otherwise we shoud
type in the Qt Command Prompt from the '~/components/RTIViewer/' directory:

qmake -tp vc -recursive rtiviewerv10.pro

and open the project file.
