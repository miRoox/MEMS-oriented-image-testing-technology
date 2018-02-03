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
