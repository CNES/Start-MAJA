/*
* Copyright (C) 2020 Centre National d'Etudes Spatiales (CNES)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
/************************************************************************************************************
 *                                                                                                          *
 *                                ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo         *
 *                             o                                                                            *
 *                          o                                                                               *
 *                        o                                                                                 *
 *                      o                                                                                   *
 *                     o       ooooooo       ooooooo    o         o      oo                                 *
 *    o              o       o        o     o       o   o         o     o   o                               *
 *      o           o       o          o   o         o  o         o    o      o                             *
 *        o        o       o           o   o         o  o         o    o        o                           *
 *         o      o        o      oooo     o         o  o         o   o           o                         *
 *          o    o          o              o         o  o         o   o           o                         *
 *           o  o            o             o         o  o o      o   o          o                           *
 *            oo              oooooooo    o         o   o  oooooo   o      oooo                             *
 *                                                     o                                                    *
 *                                                     o                                                    *
 *                                                    o                            o                        *
 *                                                    o            o      oooo     o   o      oooo          *
 *                                                   o             o         o    o    o         o          *
 *                                                   o            o       ooo     o   o       ooo           *
 *                                                               o       o       o   o          o           *
 *                                                               ooooo   oooo    o   ooooo  oooo            *
 *                                                                              o                           *
 *                                                                                                          *
 ************************************************************************************************************
 *                                                                                                          *
 * Author: CS Systemes d'Information  (France)                                                              *
 *                                                                                                          *
 ************************************************************************************************************
 * HISTORIQUE                                                                                               *
 *                                                                                                          *
 * VERSION : 5.1.0 : DM : LAIG-FA-MAC-145739-CS : 9 aout 2016 : Correction qualité sur MACCS 5.1            *
 * VERSION : 5.1.0 : <TypeFT> : <NumFT> : 22 juillet 2016 : Creation
 *                                                                                                          *
 * FIN-HISTORIQUE                                                                                           *
 *                                                                                                          *
 * $Id: vnsFastLinearInterpolateImageFunction.txx 12703 2016-07-22 14:12:29Z abricier $
 *                                                                                                          *
 ************************************************************************************************************/
#ifndef __vnsFastLinearInterpolateImageFunction_txx
#define __vnsFastLinearInterpolateImageFunction_txx

#include "vnsFastLinearInterpolateImageFunction.h"

#include "itkNumericTraits.h"
#include "vnl/vnl_vector.h"
#include "vnsMacro.h"
namespace vns
{

    // Define the number of neighbors
    template<class TInputImage, class TCoordRep>
        const unsigned long FastLinearInterpolateImageFunctionBase<TInputImage, TCoordRep>::m_Neighbors = 1 << TInputImage::ImageDimension;

    template<class TInputImage, class TCoordRep>
        void
        FastLinearInterpolateImageFunctionBase<TInputImage, TCoordRep>::PrintSelf(std::ostream& os, itk::Indent indent) const
        {
            Superclass::PrintSelf(os, indent);
            os << indent << "Neighbors: " << m_Neighbors << std::endl;
        }

    template<class TInputImage, class TCoordRep>
        void
        FastLinearInterpolateImageFunction<TInputImage, TCoordRep>::PrintSelf(std::ostream& os, itk::Indent indent) const
        {
            Superclass::PrintSelf(os, indent);
        }

    template<class TInputImage, class TCoordRep>
        typename FastLinearInterpolateImageFunction<TInputImage, TCoordRep>::OutputType
        FastLinearInterpolateImageFunction<TInputImage, TCoordRep>::EvaluateAtContinuousIndex(const ContinuousIndexType & index) const
        {
            unsigned int dim; // index over dimension

            // Compute base index = closet index below point
            // Compute distance from point to base index
            signed long baseIndex[ImageDimension];
            double distance[ImageDimension];
            long tIndex;
            double l_RealValue(0.);

            for (dim = 0; dim < ImageDimension; dim++)
            {
                // The following "if" block is equivalent to the following line without
                // having to call floor.
                if (index[dim] >= 0.0)
                {
                    baseIndex[dim] = static_cast<long>(index[dim]);
                }
                else
                {
                    tIndex = static_cast<long>(index[dim]);
                    if (static_cast<double>(tIndex) != index[dim])
                    {
                        tIndex--;
                    }
                    baseIndex[dim] = tIndex;
                }
                distance[dim] = index[dim] - static_cast<double>(baseIndex[dim]);
            }

            // Interpolated value is the weighted sum of each of the surrounding
            // neighbors. The weight for each neighbor is the fraction overlap
            // of the neighbor pixel with respect to a pixel centered on point.

//        const unsigned int size = this->GetInputImage()->GetNumberOfComponentsPerPixel();
            OutputType output;
//        output.SetSize(size);
//        output.Fill(0.0);

            typedef typename itk::NumericTraits<InputPixelType>::ScalarRealType ScalarRealType;
            ScalarRealType totalOverlap = itk::NumericTraits<ScalarRealType>::Zero;

            IndexType neighIndex;
            for (unsigned int counter = 0; counter < Superclass::m_Neighbors; counter++)
            {
                double overlap(1.); // fraction overlap
                unsigned int upper = counter; // each bit indicates upper/lower neighbour
                // get neighbor index and overlap fraction
                for (dim = 0; dim < ImageDimension; dim++)
                {

                    if (upper & 1)
                    {
                        neighIndex[dim] = baseIndex[dim] + 1;
                        // Take care of the case where the pixel is just
                        // in the outer upper boundary of the image grid.

                        // Take care of the case where the pixel is just
                        // in the outer upper boundary of the image grid.
                        if (neighIndex[dim] > this->m_EndIndex[dim])
                        {
                            neighIndex[dim] = this->m_EndIndex[dim];
                        }
                        overlap = overlap * distance[dim];
                    }
                    else
                    {
                        neighIndex[dim] = baseIndex[dim];
                        // Take care of the case where the pixel is just
                        // in the outer lower boundary of the image grid.
                        // Take care of the case where the pixel is just
                        // in the outer lower boundary of the image grid.
                        if (neighIndex[dim] < this->m_StartIndex[dim])
                        {
                            neighIndex[dim] = this->m_StartIndex[dim];
                        }
                        overlap = overlap * (1.0 - distance[dim]);
                    }

                    upper >>= 1;

                }

                // Get neighbor value only if overlap is not zero
                if (overlap)
                {
                    const InputPixelType & input = this->GetInputImage()->GetPixel(neighIndex);
//                output = output + (overlap * static_cast<RealType> (input));
                    l_RealValue += (overlap * static_cast<RealType>(input));
                    totalOverlap = totalOverlap + overlap;
                }

                if (totalOverlap == 1.0)
                {
                    // Finished
                    break;
                }

            }
            output = static_cast<OutputType>(l_RealValue);
            return (output);
        }

    template<typename TPixel, unsigned int VImageDimension, class TCoordRep>
        void
        FastLinearInterpolateImageFunction<otb::VectorImage<TPixel, VImageDimension>, TCoordRep>::PrintSelf(std::ostream& os,
                itk::Indent indent) const
        {
            Superclass::PrintSelf(os, indent);
        }

    template<typename TPixel, unsigned int VImageDimension, class TCoordRep>
        typename FastLinearInterpolateImageFunction<otb::VectorImage<TPixel, VImageDimension>, TCoordRep>::OutputType
        FastLinearInterpolateImageFunction<otb::VectorImage<TPixel, VImageDimension>, TCoordRep>::EvaluateAtContinuousIndex(
                const ContinuousIndexType & index) const
        {
            // Avoid the smartpointer de-reference in the loop for
            // "return m_InputImage.GetPointer()"
            const InputImageType * const inputImagePtr = this->GetInputImage();
            /**
             * Compute base index = closet index below point
             * Compute distance from point to base index
             */
            IndexType baseIndex;
            typedef typename ContinuousIndexType::ValueType InternalComputationType;
            InternalComputationType distance[ImageDimension];
            for (unsigned int dim = 0; dim < ImageDimension; ++dim)
            {
                baseIndex[dim] = itk::Math::Floor<IndexValueType>(index[dim]);
                distance[dim] = index[dim] - static_cast<InternalComputationType>(baseIndex[dim]);
            }

            const unsigned int componentNumber = inputImagePtr->GetNumberOfComponentsPerPixel();
            OutputType output;
            output.SetSize(componentNumber);
            output.Fill(0.);
            typedef typename itk::NumericTraits<InputPixelType>::ScalarRealType ScalarRealType;
            std::vector<ScalarRealType> value(componentNumber, itk::NumericTraits<ScalarRealType>::Zero);

            for (unsigned int counter = 0; counter < Superclass::m_Neighbors; ++counter)
            {
                InternalComputationType overlap = 1.0;    // fraction overlap
                unsigned int upper = counter;  // each bit indicates upper/lower neighbour
                IndexType neighIndex(baseIndex);

                // get neighbor index and overlap fraction
                for (unsigned int dim = 0; dim < ImageDimension; ++dim)
                {
                    if (upper & 1)
                    {
                        ++(neighIndex[dim]);
                        // Take care of the case where the pixel is just
                        // in the outer upper boundary of the image grid.
                        if (neighIndex[dim] > this->m_EndIndex[dim])
                        {
                            neighIndex[dim] = this->m_EndIndex[dim];
                        }
                        overlap *= distance[dim];
                    }
                    else
                    {
                        // Take care of the case where the pixel is just
                        // in the outer lower boundary of the image grid.
                        if (neighIndex[dim] < this->m_StartIndex[dim])
                        {
                            neighIndex[dim] = this->m_StartIndex[dim];
                        }
                        overlap *= 1.0 - distance[dim];
                    }

                    upper >>= 1;
                }
//                value += static_cast<RealType>(inputImagePtr->GetPixel(neighIndex)) * overlap;
                const InputPixelType & input = inputImagePtr->GetPixel(neighIndex);
                for (unsigned int k = 0; k < componentNumber; k++)
                {
//                    output[k] = output[k] + (overlap * static_cast<ScalarRealType> (input.GetElement(k) /*[k]*/ ));
                    value[k] = value[k] + (overlap * static_cast<ScalarRealType> (input.GetElement(k) /*[k]*/));
                }
//                totalOverlap = totalOverlap + overlap;
            }

            for (unsigned int k = 0; k < componentNumber; ++k)
            {
                output.SetElement(k, value[k]);
            }
            return (output);
       }

} //namespace otb

#endif
