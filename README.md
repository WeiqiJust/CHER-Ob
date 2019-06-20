# CHER-Ob  

CHER-Ob (CULTURAL HERITAGE-Object) is an open source platform developed in an attempt to enhance analysis, evaluation, documentation, sharing and management of 3D and 2D visualizations as well as textual and conservation science data.

The development of CHER-Ob is intended to offer a flexible, expandable integrated platform for collaborative cultural heritage research. It is compatible with commonly used imaging data types (2D and 3D images, RTIs, CT) and textual information. 
CHER-Ob offers an enhanced annotation framework and metadata schema, automatic report/video generation, different roles assigned to users, bookmark, screenshot, searching, sorting and filtering options.

## Install

CHER-Ob supports the platform of windows and Mac OS X, and user can run the executable file to finish installation. The executable files are named as `setup`  under specific folder for each platform in `software`.

## Build

CHER-Ob is currently supplied as pre-built binaries for windows and Mac OS X.

### Prerequisites

- Qt (4.8.5)
- OpenCV (3.0.0)
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


### Windows

To compile the software from source, please first add following path in system environment "Path" variable (`Control Panel->System->Advanced System Settings->advanced tab->Environment Variables->System Varible->Path`):<br>

```
$(ProjectPath)\lib\OpenCV\bin
$(ProjectPath)\lib\VTK\bin
$(ProjectPath)\lib\ITK\bin
$(ProjectPath)\lib\qwt-6.1.0\lib
$(ProjectPath)\lib\openEXR-1.7.0\bin
$(QTDir)\bin
```

`$(ProjectPath)` is where the lib folder locate. 
`$(QTDir)` is where your Qt is installed.

#### Optional:

The .sln file under Windows folder has already loaded all the include files and .lib files under lib folder.<br>
If you want to recompile the libraries, please follow the instructions below to link the libraries to the solution:<br>
- Put all the compiled libraries in different separte folders with the library's name. Each library should include an include/src, lib and bin(optional) folder.<br>
- Put all the library folder under `$(ProjectPath)\lib` folder.<br>
- Open .sln file and open solution `Properties`.<br>
- Choose the configuration and platform in which you compiled all your libraries. Make sure they are consistent.
- Open `Configuration Properties->C/C++->General->Additional Include Directories` and add each library include file in the format: `..\lib\(Library Name)\include`.<br>
- Open `Configuration Properties->Linker->General->Additional Include Directories` and add each library .lib file in the format: `..\lib\(Library Name)\lib`.<br>
- Choose OK to save the configurations.<br>

All the images and icons are registered in the file `Windows/CHEROb.qrc` and `Windows/CHEROb.rc` in relative path. If you move the images or icon folder, please modify the .qrc and .rc file to correctly load images.

### Mac OS X

In order to compile the source code on Mac, you first need to install the third-party libraries. Since `itkvtkglue`, `clapack`, `armadillo`, `vcglib`, `rtiviewer` and `rtiwebmaker` include just a set of header files, you can directly use the files in `lib` folder, or your may download and place them into directories where you can locate them. For the other third-party libraries (`Qt`, `OpenCV`, `VTK`, `ITK`, `QWT`, `openEXR`), you may first download the source code from their respective official sites and then follow their provided instructions to carry out compilations and installations. 

To facilitate compilation, an example Qt project file (CHER-Ob.pro) is also provided, but you may have to modify some lines to include the correct paths to the required libraries.

## License

GNU General Public License Usage

Alternatively, this file may be used under the terms of the GNU General Public License version 3.0 as published by the Free Software Foundation and appearing in the file LICENSE.
GPL included in the packaging of this file. Please review the following information to ensure the GNU General Public License version 3.0 requirements will be met:
* http://www.gnu.org/copyleft/gpl.html.


## Contributor

**Yale Computer Graphics Group**

- Weiqi Shi (weiqi.shi@yale.edu)
- Zeyu Wang (zeyu.wang@yale.edu)
- Ying Yang (ying.yang.yy368@yale.edu)
- Min H. Kim (minhkim@cs.yale.edu)
- David Tidmarsh (tidmarsh@aya.yale.edu)
- Eleni Kotoula (eleni.kotoula@yale.edu)
- Kiraz Akoglu (goze.akoglu@yale.edu)
- Holly Rushmeier (holly.rushmeier@yale.edu)

## Contact

If you have any problem or suggestion related to the software, please send an email to cher_ob@cs.yale.edu.

## Acknowledgments

Some portions of this file are based on the example codes of ITK/VTK library from Kitware, Qt API from Nokia, RTIViewer from Visual Computing Lab, Meshlab VCG library from ISTI CNR, OpenCV library and Google Maps API.<br>
We would like to thank anonymous help by various software engineering communities.

## References

Wang, Zeyu et al. 2018. “[CHER-Ob: A Tool for Shared Analysis and Video Dissemination.](http://graphics.cs.yale.edu/site/publications/cher-ob-tool-shared-analysis-and-video-dissemination)” ACM Journal on Computing and Cultural Heritage (2018).<br>
Wang, Zeyu et al. 2017. “[An Introductory Video Generator for Disseminating Cultural Heritage Projects.](http://graphics.cs.yale.edu/site/publications/introductory-video-generator-disseminating-cultural-heritage-projects)” Proceedings of EUROGRAPHICS Workshop on Graphics and Cultural Heritage (2017).<br>
Shi, Weiqi et al. 2016. “[CHER-Ob: A Tool for Shared Analysis in Cultural Heritage.](http://graphics.cs.yale.edu/site/publications/cher-ob-tool-shared-analysis-cultural-heritage)” Proceedings of EUROGRAPHICS Workshop on Graphics and Cultural Heritage (2016).<br>
Kim, Min H. et al. 2014. “[Hyper3D: 3D Graphics Software for Examining Cultural Artifacts.](http://graphics.cs.yale.edu/site/publications/hyper3d-3d-graphics-software-examining-cultural-artifacts)” Journal Article: ACM Journal of Computing and Cultural Heritage, vol. 7, issue 3 (2014).<br>
Kim, Min H. et al. 2012. “[3D Imaging Spectroscopy for Measuring Hyperspectral Patterns on Solid Objects.](http://graphics.cs.yale.edu/site/publications/3d-imaging-spectroscopy-measuring-hyperspectral-patterns-solid-objects)” ACM Transactions on Graphics 31.4 (July 2012). Article 38.<br>
Palma, Gianpaolo et al. 2010. "[Dynamic shading enhancement for reflectance transformation imaging.](https://dl.acm.org/citation.cfm?id=1841321)" Journal Article: ACM Journal of Computing and Cultural Heritage, vol 3, issue 2 (2010).
