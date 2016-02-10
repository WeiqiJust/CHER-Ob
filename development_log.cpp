/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

 - License:  GNU General Public License Usage
   Alternatively, this file may be used under the terms of the GNU General
   Public License version 3.0 as published by the Free Software Foundation
   and appearing in the file LICENSE.GPL included in the packaging of this
   file. Please review the following information to ensure the GNU General
   Public License version 3.0 requirements will be met:
   http://www.gnu.org/copyleft/gpl.html.

 - Warranty: This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.

*****************************************************************************/
/*

======================================================================================
Unfixed bug list
======================================================================================
- PLY and WRL file format has some issue with texture data
  (when the object is rotated with texture, there is a significant memory leak happen)
- Single scan PLY file cannot be read like the orignal single view of Bunny.
======================================================================================
TODO list
======================================================================================
- add texture file resolution
- add manual
- add mesh label annotation
- add RTI reading and visualization
- make an xml IO for project files
- add texture mapping tool
- add contrast/brightness control for volume rendering
- cut the inheritance of vtkWidget
- add cropping tool for CT
- add spin animation button
- fix the texture mapping bug for split windows
- fix the screen refresh update issue in the spin annimation
- add the progress bar for file reading

======================================================================================
Unfixed bug list
======================================================================================
- fix the bug of camera up vector transform issue when controling the light direction.
- light control coordinates are not correct.

======================================================================================
Fixed bug list
======================================================================================
(6/4/12) - 3DS and WRL couldn't be read.
(6/4/12) - when the visualization is set to 3D volume, when you read a file, the software cannot figure out the current option.
(6/5/12) - on mac, GPU volume rendering on mac (vtkBoxWidget) doesn't work.
(6/5/12) - when the ct visualization mode is changed from volume rendering to 2d stack view,
   the 2d stack view works very slowly
(6/6/12) - when opening a new file, volume rendering mode is changed
(6/11/12) - not compilable on PC
(6/28/12) - OBJ and PLY low-level texture file reading was finished.
(7/5/12) - add openEXR
(7/5/12) - add WRML Texture mapping
(7/5/12) - add hyperspectral data reading
(7/5/12) - reconnect user interface
(7/5/12) - rewrite 3D files reading functions (parser of 3D image data) and bridged to VTK pipeline
(7/5/12) - fix world to display coordinates conversion.s
(7/5/12) - information panel and light doesn't show up initially.
(7/9/12) - fix the bug of camera up vector transform issue when controling the light direction.
(7/9/12) - light control coordinates are not correct.
(7/9/12) - add information on/off switch
(7/9/12) - add different rendering mode
(7/18/12) - vtk object separated from the multi-document child window when it is displayed seperately.
(7/18/12) - vtk libary vtkImageData (including pipeline) crashes in the deploy library settings
            this happens on OpenEXR, OpenPLY, OpenVRML classes.

 */
