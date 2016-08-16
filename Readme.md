#CHER-Ob  

CHER-Ob (CULTURAL HERITAGE-Object) is an open source platform developed in an attempt to enhance analysis, evaluation, documentation, sharing and management of 3D and 2D visualizations as well as textual and conservation science data.

The development of CHER-Ob is intended to offer a flexible, expandable integrated platform for collaborative cultural heritage research. It is compatible with commonly used imaging data types (2D and 3D images, RTIs, CT) and textual information. 
CHER-Ob offers an enhanced annotation framework and metadata schema, automatic report generation, different roles assigned to users, bookmark, screenshot, searching, sorting and filtering options.

##Install

CHER-Ob is currently supplied as pre-built binaries for windows. 

###Prerequisites
- Qt
- VTK
- ITK
- openEXR
- rtiviewer
- itkvtkglue
- clapack
- armadillo
- vcglib
- rtiwebmaker

###Windows

To use the software, please run `setup.exe` under `software/windows` folder.

To compile the software from source, please first add following path in system environment "Path" variable (`Control Panel->System->Advanced System Settings->advanced tab->Environment Variables->System Varible->Path`): <br />

```
$(ProjectPath)\lib\ITK\bin
$(ProjectPath)\lib\VTK\bin
$(ProjectPath)\lib\openEXR-1.7.0\bin
$(ProjectPath)\lib\qwt-6.1.0\lib
$(QTDir)\bin
```

`$(ProjectPath)` is where the lib folder locate. 
`$(QTDir)` is where your QT is installed.

####Optional:
The .sln file under Visual Studio folder has already loaded all the include files and .lib files under lib folder.<br />
If you want to recompile the libraries, please follow the instructions below to link the libraries to the solution:<br />
- Put all the compiled libraries in different separte folders with the library's name. Each library should include an include/src, lib and bin(optional) folder.<br />
- Put all the library folder under `$(ProjectPath)\lib` folder.<br />
- Open .sln file and open solution `Properties`.<br />
- Choose the configuration and platform in which you compiled all your libraries. Make sure they are consistent.
- Open `Configuration Properties->C/C++->General->Additional Include Directories` and add each library include file in the format: `..\lib\(Library Name)\include`.<br />
- Open `Configuration Properties->Linker->General->Additional Include Directories` and add each library .lib file in the format: `..\lib\(Library Name)\lib`.<br />
- Choose OK to save the configurations.<br />

All the images and icons are registered in the file `Visual Studio/CHEROb.qrc` and `Visual Studio/CHEROb.rc` in relative path. If you move the images or icon folder, please modify the .qrc and .rc file to correctly load images.

##License
GNU General Public License Usage
Alternatively, this file may be used under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation and appearing in the file LICENSE.GPL included in the packaging of this file. Please review the following information to ensure the GNU General Public License version 3.0 requirements will be met:
* http://www.gnu.org/copyleft/gpl.html.


##Contributor
**Yale Computer Graphics Group**<br />
- Weiqi Shi (weiqi.shi@yale.edu)
- Ying Yang (ying.yang.yy368@yale.edu)
- Min H. Kim (minhkim@cs.yale.edu)
- David Tidmarsh (tidmarsh@aya.yale.edu)
- Eleni Kotoula (eleni.kotoula@yale.edu)
- Kiraz Akoglu (goze.akoglu@yale.edu)
- Holly Rushmeier (holly.rushmeier@yale.edu)

##Acknowledgments
Some portions of this file are based on the example codes of ITK/VTK library from Kitware, QT API from Nokia, RTIViewer from Visual Computing Lab and Meshlab VCG library from ISTI CNR. We would like to thank anonymous help by various software engineering communities.