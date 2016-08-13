/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkAnchorErodeDilateImageFilter_h
#define __itkAnchorErodeDilateImageFilter_h

#include "itkKernelImageFilter.h"
#include "itkProgressReporter.h"
#include "itkAnchorErodeDilateLine.h"
#include "itkBresenhamLine.h"

namespace itk
{
/**
 * \class AnchorErodeDilateImageFilter
 * \brief class to implement erosions and dilations using anchor
 * methods. This is the base class that must be instantiated with
 * appropriate definitions of greater, less and so on.
 * The SetBoundary facility isn't necessary for operation of the
 * anchor method but is included for compatibility with other
 * morphology classes in itk.
 * \ingroup ITKMathematicalMorphology
 */
template< class TImage, class TKernel,
          class TFunction1 >
class ITK_EXPORT AnchorErodeDilateImageFilter:
  public KernelImageFilter< TImage, TImage, TKernel >
{
public:
  /** Standard class typedefs. */
  typedef AnchorErodeDilateImageFilter         Self;
  typedef KernelImageFilter< TImage, TImage, TKernel >
                                               Superclass;
  typedef SmartPointer< Self >                 Pointer;
  typedef SmartPointer< const Self >           ConstPointer;

  /** Some convenient typedefs. */
  /** Kernel typedef. */
  typedef TKernel KernelType;

  typedef TImage                                InputImageType;
  typedef typename InputImageType::Pointer      InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename InputImageType::RegionType   InputImageRegionType;
  typedef typename InputImageType::PixelType    InputImagePixelType;
  typedef typename TImage::IndexType            IndexType;
  typedef typename TImage::SizeType             SizeType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(AnchorErodeDilateImageFilter,
               KernelImageFilter);

  /** Set/Get the boundary value. */
  itkSetMacro(Boundary, InputImagePixelType);
  itkGetConstMacro(Boundary, InputImagePixelType);

protected:
  AnchorErodeDilateImageFilter();
  ~AnchorErodeDilateImageFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData(const InputImageRegionType & outputRegionForThread,
                             ThreadIdType threadId);

  // should be set by the meta filter
  InputImagePixelType m_Boundary;

private:
  AnchorErodeDilateImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);               //purposely not implemented

  typedef BresenhamLine< itkGetStaticConstMacro(InputImageDimension) > BresType;

  // the class that operates on lines
  typedef AnchorErodeDilateLine< InputImagePixelType, TFunction1 > AnchorLineType;
}; // end of class
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAnchorErodeDilateImageFilter.hxx"
#endif

#endif
