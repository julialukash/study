#ifndef DERIVATOR_H
#define DERIVATOR_H

#include <algorithm>

#include "interface.h"
#include "processors_data.h"

//#define DEBUG_MODE = 1

class ApproximateOperations
{
private:
    const NetModel& netModel;
    const ProcessorsData& processorData;
public:
    ApproximateOperations(const NetModel& model, const ProcessorsData& processorData):
        netModel(model), processorData(processorData)
    {
    }

    // note: calculates -laplass(currentValues)
    std::shared_ptr<DoubleMatrix> CalculateLaplass(const DoubleMatrix& currentValues) const
    {        
#ifdef DEBUG_MODE
        std::cout <<"ApproximateOperations.CalculateLaplass currentValues = \n" << currentValues << std::endl;
        std::cout <<"ApproximateOperations.CalculateLaplass startIndex = " << startIndex << ", endIndex = " << endIndex << std::endl;
#endif
        auto laplassValues = std::make_shared<DoubleMatrix>(processorData.RowsCountWithBorders(), processorData.ColsCountWithBorders());
#ifdef DEBUG_MODE
        std::cout <<"ApproximateOperations.CalculateLaplass laplassValues = \n" << laplassValues << std::endl;
#endif
        for (int i = processorData.FirstInnerRowRelativeIndex(); i <= processorData.LastInnerRowRelativeIndex(); ++i)
        {
            for (int j = processorData.FirstInnerColRelativeIndex(); j <= processorData.LastInnerColRelativeIndex(); ++j)
            {
                double xPart = (currentValues(i, j) - currentValues(i, j - 1)) / netModel.xStep(j - 1) -
                             (currentValues(i, j + 1) - currentValues(i, j)) / netModel.xStep(j);
                double yPart = (currentValues(i, j) - currentValues(i - 1, j)) / netModel.yStep(i - 1) -
                             (currentValues(i + 1, j) - currentValues(i, j)) / netModel.yStep(i);
                (*laplassValues)(i, j) = xPart / netModel.xAverageStep(j) + yPart / netModel.yAverageStep(i);
#ifdef DEBUG_MODE
                std::cout <<"ApproximateOperations.CalculateLaplass i = " << i << ", iNetIndex = " << iNetIndex << ", j = " << j << ", value = " << laplassValues(i, j) << std::endl;
#endif
            }
        }
#ifdef DEBUG_MODE
        std::cout <<"ApproximateOperations.CalculateLaplass laplassValues = \n" << laplassValues << std::endl;
#endif
        return laplassValues;
    }

    double ScalarProduct(const DoubleMatrix& currentValues, const DoubleMatrix& otherValues) const
    {
        double prodValue = 0;
        for (int i = processorData.FirstInnerRowRelativeIndex(); i <= processorData.LastInnerRowRelativeIndex(); ++i)
        {
            for (int j = processorData.FirstInnerColRelativeIndex(); j <= processorData.LastInnerColRelativeIndex(); ++j)
            {
                prodValue = prodValue + netModel.xAverageStep(j) * netModel.yAverageStep(i) *
                                        currentValues(i, j) * otherValues(i, j);
            }
        }
        return prodValue;
    }


    double MaxNormValue(const DoubleMatrix& currentValues) const
    {
        auto minMax = std::minmax_element(&(currentValues(0,0)),
                        &(currentValues(0,0)) + currentValues.rowsCount() * currentValues.colsCount());
        double min = fabs(*minMax.first);
        double max = fabs(*minMax.second);
#ifdef DEBUG_MODE
        std::cout << "MaxNormValue min = " << min << ", max = " << max << std::endl;
#endif
        return max > min ? max : min;
    }

};

#endif // DERIVATOR_H
