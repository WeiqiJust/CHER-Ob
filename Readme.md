#CHER-Ob  
**Yale Computer Graphics Group**

**License:**<br />
GNU General Public License Usage
Alternatively, this file may be used under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation and appearing in the file LICENSE.GPL included in the packaging of this file. Please review the following information to ensure the GNU General Public License version 3.0 requirements will be met:
* http://www.gnu.org/copyleft/gpl.html.

**Warranty:**<br />
This software is distributed WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

**Contributor:**<br />
Weiqi Shi (weiqi.shi@yale.edu)<br />
Ying Yang (ying.yang.yy368@yale.edu)<br />
Min H. Kim (minhkim@cs.yale.edu)<br />
David Tidmarsh (tidmarsh@aya.yale.edu)<br />

Installation for Windows
========================

Please add following path in system environment "Path" variable (Control Panel->System->Advanced System Settings->advanced tab->Environment Variables->System Varible->Path): 
$(ProjectPath)\lib\ITK\bin;
$(ProjectPath)\lib\VTK\bin;
$(ProjectPath)\lib\openEXR-1.7.0\bin;
$(ProjectPath)\lib\qwt-6.1.0\lib
$(QTDir)\bin

$(ProjectPath) is where the lib folder locate. $(QTDir) is where your QT is installed.

Optional:
The .sln file under Visual Studio folder has already loaded all the include files and .lib files under lib folder.
If you recompile the libraries, please follow the instructions below to link the libraries:
1. Put all the compiled libraries in different separte folders with the library's name. Each library should include an include/src, lib and bin(optional) folder.
2. Put all the library folder under $(ProjectPath)\lib folder.
3. Open .sln file and open solution "Properties".
4. Choose the configuration and platform in which you compiled all your libraries. Make sure they are consistent.
5. Open Configuration Properties->C/C++->General->Additional Include Directories and add each library include file in the format: ..\lib\(Library Name)\include.
6. Open Configuration Properties->Linker->General->Additional Include Directories and add each library .lib file in the format: ..\lib\(Library Name)\lib.
7. Choose OK to save the configurations.

All the images and icons are registered in the file Visual Studio/CHEROb.qrc and Visual Studio/CHEROb.rc in relative path. If you move the images or icon folder, please modify the .qrc and .rc file to correctly load images.

The compiled software can be found in folder software/windows/CHER-Ob.exe

**Acknowledgments:**<br />
Some portions of this file are based on the example codes of ITK/VTK library from Kitware, QT API from Nokia, RTIViewer from Visual Computing Lab and Meshlab VCG library from ISTI CNR. We would like to thank anonymous help by various software engineering communities.