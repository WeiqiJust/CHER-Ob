#CHER-Ob  

CHER-Ob (CULTURAL HERITAGE-Object) is an open source platform developed in an attempt to enhance analysis, evaluation, documentation, sharing and management of 3D and 2D visualizations as well as textual and conservation science data.

The development of CHER-Ob is intended to offer a flexible, expandable integrated platform for collaborative cultural heritage research. It is compatible with commonly used imaging data types (2D and 3D images, RTIs, CT) and textual information. 
CHER-Ob offers an enhanced annotation framework and metadata schema, automatic report generation, different roles assigned to users, bookmark, screenshot, searching, sorting and filtering options.

##Install

CHER-Ob supports the platform of windows and Mac OS X, and user can run the executable file to finish installation. The executable files are named as `setup`  under specific folder for each platform in `software`.

##Build

CHER-Ob is currently supplied as pre-built binaries for windows and Mac OS X

###Prerequisites
- Qt (4.8.5)
- VTK (5.10)
- ITK (4.4)
- QWT (6.1.0)
- openEXR (1.7.0)
- rtiviewer
- itkvtkglue
- clapack
- armadillo
- vcglib
- rtiwebmaker

Note that the numbers in the round brackets indicate the versions of third-party libraries we used to compile the source code and that you may encounter compatibility issues when using lower/higher versions. Also note that the provided libraries were built with 64 bits using Visual Studio 2008, so you may need to re-compile if you are using 32 bits and a different compiler. 


###Windows

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
The .sln file under Windows folder has already loaded all the include files and .lib files under lib folder.<br />
If you want to recompile the libraries, please follow the instructions below to link the libraries to the solution:<br />
- Put all the compiled libraries in different separte folders with the library's name. Each library should include an include/src, lib and bin(optional) folder.<br />
- Put all the library folder under `$(ProjectPath)\lib` folder.<br />
- Open .sln file and open solution `Properties`.<br />
- Choose the configuration and platform in which you compiled all your libraries. Make sure they are consistent.
- Open `Configuration Properties->C/C++->General->Additional Include Directories` and add each library include file in the format: `..\lib\(Library Name)\include`.<br />
- Open `Configuration Properties->Linker->General->Additional Include Directories` and add each library .lib file in the format: `..\lib\(Library Name)\lib`.<br />
- Choose OK to save the configurations.<br />

All the images and icons are registered in the file `Windows/CHEROb.qrc` and `Windows/CHEROb.rc` in relative path. If you move the images or icon folder, please modify the .qrc and .rc file to correctly load images.

###Mac OS X

In order to compile the source code on Mac, you first need to install the third-party libraries. Since `itkvtkglue`, `clapack`, `armadillo`, `vcglib`, `rtiviewer` and `rtiwebmaker` include just a set of header files,
you can directly use the files in `lib` folder, or your may download and place them into directories where you can locate them. For the other third-party libraries (`QT`, `VTK`, `ITK`, `QWT`, `openEXR`), you may first
download the source code from their respective official sites and then follow their provided instructions to carry out compilations and installations. 

To facilitate compilation, an example QT project file (CHER-Ob.pro) is also provided, but you may have to modify some lines to include the correct paths to the required libraries.

##License
GNU General Public License Usage
Alternatively, this file may be used under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation and appearing in the file LICENSE.
GPL included in the packaging of this file. Please review the following information to ensure the GNU General Public License version 3.0 requirements will be met:
* http://www.gnu.org/copyleft/gpl.html.


##Contributor
**Yale Computer Graphics Group**<br />
- Weiqi Shi (weiqi.shi@yale.edu)
- Ying Yang (ying.yang.yy368@yale.edu)
- Zeyu Wang (zeyu.wang@yale.edu)
- Min H. Kim (minhkim@cs.yale.edu)
- David Tidmarsh (tidmarsh@aya.yale.edu)
- Eleni Kotoula (eleni.kotoula@yale.edu)
- Kiraz Akoglu (goze.akoglu@yale.edu)
- Holly Rushmeier (holly.rushmeier@yale.edu)

##Contact
If you have any problem or suggestion related to the software, please send email to cher_ob@cs.yale.edu.

##Acknowledgments
Some portions of this file are based on the example codes of ITK/VTK library from Kitware, QT API from Nokia, RTIViewer from Visual Computing Lab and Meshlab VCG library from ISTI CNR. 
We would like to thank anonymous help by various software engineering communities.