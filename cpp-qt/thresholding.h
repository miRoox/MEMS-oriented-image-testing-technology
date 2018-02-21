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


#ifndef THRESHOLDING_H
#define THRESHOLDING_H

#include <QtGlobal>

class QImage;

namespace MEMS {

enum class AutoThresholdMethod
{
    Cluster     = 0,
    Otsu        = AutoThresholdMethod::Cluster,
    Mean        = 1,
    Moments     = 2,
    Fuzziness   = 3,
};

using Histogram = QVector<::std::size_t>;

extern int meanThreshold(const QImage& image);
extern int meanThreshold(const Histogram& histogram);
extern int pTileThreshold(const QImage& image, qreal pValue);
extern int pTileThreshold(const Histogram& histogram, qreal pValue);
extern int clusterThreshold(const QImage& image);
extern int clusterThreshold(const Histogram& histogram);
extern int momentsThreshold(const QImage& image);
extern int momentsThreshold(const Histogram& histogram);
extern int fuzzinessThreshold(const QImage& image);
extern int fuzzinessThreshold(const Histogram& histogram);

extern Histogram grayscaleHistogram(const QImage& image);

} // namespace MEMS

#endif // THRESHOLDING_H
