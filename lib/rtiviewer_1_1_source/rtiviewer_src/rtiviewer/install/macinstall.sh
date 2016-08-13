#!/bin/bash
# this is a script shell for setting up the application bundle for the mac
# it should be run in the rtiviewer/install dir.
# it moves plugins and frameworks into the package and runs the 
# install_tool on them to change the linking path to the local version of qt
# the build was issued with
# qmake "CONFIG += debug_and_release warn_off" rtiviewer.pro -spec macx-g++
# make clean
# make release
# Note that sometimes you have to copy by hand the icons in the rtiviewer.app/Contents/Resources directory

cd ../src

QTPATH="/Library/Frameworks"

QTIMAGELIBSPATH="/Developer/Applications/Qt/plugins/imageformats"

APPNAME="RTIViewer.app"
APPNAME2="rtiwebmaker"
BUNDLE="RTIViewerBundle"

QTCOMPONENTS="QtCore QtGui QtOpenGL QtNetwork QtXml"

QTCORE=QtCore.framework/Versions/4/QtCore
QTGUI=QtGui.framework/Versions/4/QtGui
QTNETWORK=QtNetwork.framework/Versions/4/QtNetwork
QTXML=QtXml.framework/Versions/4/QtXml
QTOPENGL=QtOpenGL.framework/Versions/4/QtOpenGL

if [ -e ./bin/$APPNAME ]
then
  echo "------------------"
else
  echo "Started in the wrong dir"
  exit 0
fi

if [ -e ../../rtiwebmaker/src/bin/$APPNAME2 ]
then
	echo "------------------"
else
	echo "Started in the wrong dir"
	exit 0
fi

echo "Starting to copying stuff in the bundle"

rm -r -f $BUNDLE

mkdir $BUNDLE
cp -r bin/$APPNAME $BUNDLE

mkdir $BUNDLE/$APPNAME/Contents/MacOS

cp -r ../../rtiwebmaker/src/bin/rtiwebmaker $BUNDLE/$APPNAME/Contents/MacOS

cp -r ../doc/manual.pdf $BUNDLE

cp -r ../doc/gpl.txt $BUNDLE
cp -r ../doc/readme.txt $BUNDLE
cp -r ../doc/thirdpartycode.txt $BUNDLE

mkdir $BUNDLE/$APPNAME/Contents/Resources
# we must add also a qt.conf that contains the new dir of the plugins
cp ../install/qt.conf $BUNDLE/$APPNAME/Contents/Resources

# copy the files icons into the app.
cp images/rtiviewer.icns $BUNDLE/$APPNAME/Contents/Resources

mkdir $BUNDLE/$APPNAME/Contents/Frameworks   
mkdir $BUNDLE/$APPNAME/Contents/plugins   
mkdir $BUNDLE/$APPNAME/Contents/plugins/imageformats

for x in $QTCOMPONENTS
do
  rsync -avu --exclude='*debug*' $QTPATH/$x.framework $BUNDLE/$APPNAME/Contents/Frameworks
done

echo "now trying to change the paths in the rtiviewer executable"

for x in $QTCOMPONENTS
do
  install_name_tool -id  @executable_path/../Frameworks/$x.framework/Versions/4/$x $BUNDLE/$APPNAME/Contents/Frameworks/$x.framework/Versions/4/$x
done

install_name_tool -change $QTCORE @executable_path/../Frameworks/$QTCORE $BUNDLE/$APPNAME/Contents/Frameworks/$QTGUI
  
install_name_tool -change $QTCORE @executable_path/../Frameworks/$QTCORE $BUNDLE/$APPNAME/Contents/Frameworks/$QTXML

install_name_tool -change $QTCORE @executable_path/../Frameworks/$QTCORE $BUNDLE/$APPNAME/Contents/Frameworks/$QTNETWORK

install_name_tool -change $QTCORE @executable_path/../Frameworks/$QTCORE $BUNDLE/$APPNAME/Contents/Frameworks/$QTOPENGL

install_name_tool -change $QTGUI @executable_path/../Frameworks/$QTGUI $BUNDLE/$APPNAME/Contents/Frameworks/$QTOPENGL


IMAGEFORMATSPLUGINS="libqjpeg.dylib libqgif.dylib libqtiff.dylib"
for x in $IMAGEFORMATSPLUGINS
do
  cp $QTIMAGELIBSPATH/$x $BUNDLE/$APPNAME/Contents/plugins/imageformats
  install_name_tool -change $QTCORE  @executable_path/../Frameworks/$QTCORE  $BUNDLE/$APPNAME/Contents/plugins/imageformats/$x 
  install_name_tool -change $QTGUI   @executable_path/../Frameworks/$QTGUI   $BUNDLE/$APPNAME/Contents/plugins/imageformats/$x 
done

echo "Now Changing " #--------------------------

EXECNAMES="MacOS/rtiviewer" 
for x in $EXECNAMES
do
  install_name_tool -change $QTCORE @executable_path/../Frameworks/$QTCORE       $BUNDLE/$APPNAME/Contents/$x
  install_name_tool -change $QTGUI @executable_path/../Frameworks/$QTGUI         $BUNDLE/$APPNAME/Contents/$x
  install_name_tool -change $QTNETWORK @executable_path/../Frameworks/$QTNETWORK $BUNDLE/$APPNAME/Contents/$x
  install_name_tool -change $QTOPENGL @executable_path/../Frameworks/$QTOPENGL   $BUNDLE/$APPNAME/Contents/$x
  install_name_tool -change $QTXML @executable_path/../Frameworks/$QTXML         $BUNDLE/$APPNAME/Contents/$x
done


EXECNAMES="MacOS/rtiwebmaker" 
for x in $EXECNAMES
do
  install_name_tool -change $QTCORE @executable_path/../Frameworks/$QTCORE       $BUNDLE/$APPNAME/Contents/$x
  install_name_tool -change $QTGUI @executable_path/../Frameworks/$QTGUI         $BUNDLE/$APPNAME/Contents/$x
  install_name_tool -change $QTXML @executable_path/../Frameworks/$QTXML         $BUNDLE/$APPNAME/Contents/$x
done

echo "Create link"
cd ./$BUNDLE
ln -s ./$APPNAME/Contents/MacOS/RTIWebMaker ./
cd ../

cd ../install 
echo "Done"

#Suppose you have:
#1) Folder of content
#2) Background image file for that folder, in the folder.
#3) Icon for dmg, anywhere.
#To make disk image (with background, and with custom image icon):
# 1) Open Disk Utility
# 2) Images-->New-->Blank Image
#   a) leave it on read/write disk image
#   b) leave it on Encryption: none
#   c) Size: select a size somewhat larger than the size of the folder (since the available space is less when formatted).
#   d) Save As: This is the name for the mounted Image. 
#      ***** Choose a good VOLUME NAME for your disk image. It can contains spaces, etc., but you will not be # able to change this later 
#      (You will only be able to change name of .dmg file).
# 3) Move your folder contents into the empty disk image that you just created (and it automatically mounts after the previous step).
# 4) From the finder while you are viewing the opened image, Apple-J (View-->Show View Options)
#   a) Use the icon view, Click the capsule to hide the toolbar.
#   b) Select: This Window Only
#   c) Select desired icon size
#   d) Select Background: Picture, then choose the file that is *INSIDE* your disk image. There are a variety of ways to hide this file, such as # in hidden folders beginning with ".", or using blank or camoflauged icons and space " " file names with hidden extensions in Get Info.
# 5) Apply the custom icon to your disk image, if desired. (If the icon is applied to another file or folder, just ÒGet InfoÓ on that item, click the icon in the upper-left corner of the window, and press Command-C to copy the icon. Click the icon in the ÒGet InfoÓ window of your target disk, and paste the icon with Command-V.).
# 6) Arrange icons & window size as desired, and close window.
# 7) Go back to Disk Utility, and in the left window click to select the .dmg (not the disk image that is hanging off of it).
# 8 ) Select Images--> Convert...
# 9) Choose a new filename for the converted (final) .dmg file. Note that the name of the mounted image itself will remain unchanged.
#   a) Select Compressed, then go ahead & convert.
# 10) Voila, you are finished.