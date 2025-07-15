#include "Plotting/utils.h"

double random_zero_to_one()
{
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    // ready to generate random numbers
    double currentRandomNumber = unif(rng);
    return currentRandomNumber;
}

double scalePoint(double raw, double minValue, double maxValue)
{
    double absMinValue = fabs(minValue);
    double shiftedMax = maxValue + absMinValue;

    return (raw + absMinValue) / shiftedMax - 0.5;
}

// goes from [-scaleEdgeValue, scaleEdgeValue] to [minValue, maxValue]
double scaleBackPoint(double scaled, double scaleEdgeValue, double minValue, double maxValue)
{
    double absMinValue = fabs(minValue);
    double shiftedMax = maxValue - absMinValue;
    // return (fabs(scaled) - scaleEdgeValue) * shiftedMax * scaled / fabs(scaled) + minValue;
    return (scaled + scaleEdgeValue) * shiftedMax + minValue;
}

std::vector<Vertex> scaleVector(std::vector<Vertex> vect, double xMinValue, double xMaxValue, double yMinValue, double yMaxValue)
{
    // scale vector such that all new values would be [-0.5, 0.5]
    double xAbsMinValue = fabs(xMinValue);
    double xShiftedMax = xMaxValue + xAbsMinValue;

    double yAbsMinValue = fabs(yMinValue);
    double yShiftedMax = yMaxValue + yAbsMinValue;

    std::vector<Vertex> scaledVector;

    for (Vertex vert : vect)
    {
        double shiftedX = scalePoint(vert.x, xMinValue, xMaxValue);
        double shiftedY = scalePoint(vert.y, yMinValue, yMaxValue);
        Vertex scaledVertex{shiftedX, shiftedY};
        scaledVector.push_back(scaledVertex);
    }

    return scaledVector;
}

bool comp_x(const Vertex &a, const Vertex &b)
{
    return a.x < b.x;
}

bool comp_y(const Vertex &a, const Vertex &b)
{
    return a.y < b.y;
}
