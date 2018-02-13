/**
 ** MIT License
 **
 ** This file is part of the MEMS-oriented-image-testing-technology project.
 ** Copyright (c) 2018 Lu <miroox@outlook.com>.
 **
 ** Permission is hereby granted, free of charge, to any person obtaining a copy
 ** of this software and associated documentation files (the "Software"), to deal
 ** in the Software without restriction, including without limitation the rights
 ** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 ** copies of the Software, and to permit persons to whom the Software is
 ** furnished to do so, subject to the following conditions:
 **
 ** The above copyright notice and this permission notice shall be included in all
 ** copies or substantial portions of the Software.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 ** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
 ** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 ** SOFTWARE.
 **/


#include "edgedetect.h"
#include "imagefilter.h"

namespace MEMS {

QImage sobelOperator(const QImage& img)
{
    static const MatrixKernel sobelX{{ {{-1, 0, 1}},
                                       {{-2, 0, 2}},
                                       {{-1, 0, 1}} }};
    static const MatrixKernel sobelY{{ {{ 1, 2, 1}},
                                       {{ 0, 0, 0}},
                                       {{-1,-2,-1}} }};
    return convolveXY(img,sobelX,sobelY,PaddingType::Fixed);
}

QImage prewittOperator(const QImage& img)
{
    static const MatrixKernel prewittX{{ {{-1, 0, 1}},
                                         {{-1, 0, 1}},
                                         {{-1, 0, 1}} }};
    static const MatrixKernel prewittY{{ {{-1,-1,-1}},
                                         {{ 0, 0, 0}},
                                         {{ 1, 1, 1}} }};
    return convolveXY(img,prewittX,prewittY,PaddingType::Fixed);
}

QImage scharrOperator(const QImage& img)
{
    static const MatrixKernel scharrX{{ {{  3, 10,  3}},
                                        {{  0,  0,  0}},
                                        {{ -3,-10, -3}} }};
    static const MatrixKernel scharrY{{ {{  3,  0, -3}},
                                        {{ 10,  0,-10}},
                                        {{  3,  0, -3}} }};
    return convolveXY(img,scharrX,scharrY,PaddingType::Fixed);
}

QImage laplacianOperator(const QImage& img)
{
    static const MatrixKernel laplacian{{ {{ 0, 1, 0}},
                                          {{ 1,-4, 1}},
                                          {{ 0, 1, 0}} }};
    return convolve(img,laplacian,PaddingType::Fixed);
}

} // namespace MEMS
