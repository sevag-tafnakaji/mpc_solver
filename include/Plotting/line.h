#include <vector>
#include <random>
#include <chrono>

#include "colour.h"

#define NUM_COLOURS 4

struct Vertex
{
    double x;
    double y;

    Vertex(double _x, double _y) : x{_x}, y{_y} {}
};

struct RenderData
{
    unsigned int VAO;
    int bufferSize;
    Eigen::Vector3f colour;
};

/**
 * TODO: Add following options:
 * Varied forms of dashed lines
 * lines with (textured) points on given vertices?
 */
class Line
{
public:
    std::vector<Vertex> lineData;
    LineColour lineColour;

    Line(std::vector<Vertex> _lineData, LineColour _lineColour) : lineData{_lineData}, lineColour{_lineColour} {}

    Line(std::vector<Vertex> _lineData)
    {
        lineData = _lineData;
    }

    Line(std::vector<double> x, std::vector<double> y, Eigen::Vector3f _lineColour)
    {
        setData(x, y);
        lineColour.setLineColour(_lineColour);
    }

    Line(std::vector<double> x, std::vector<double> y, std::string _lineColour)
    {
        setData(x, y);
        lineColour.setLineColour(_lineColour);
    }

    Line(int numLines)
    {
        lineColour.setLineColour(validColours[numLines]);
    }

    void setLineColour(std::string _colour)
    {
        lineColour.setLineColour(_colour);
    }

    void setLineColour(float r, float g, float b)
    {
        lineColour.setLineColour(r, g, b);
    }

    void setLineColour(Eigen::Vector3f _colour)
    {
        lineColour.setLineColour(_colour[0], _colour[1], _colour[2]);
    }

    void setData(std::vector<Vertex> _lineData)
    {
        lineData = _lineData;
    }

    void setData(std::vector<double> x, std::vector<double> y)
    {
        if (x.size() != y.size())
            throw std::invalid_argument("X & Y are of different sizes.");

        for (int i = 0; i < (int)x.size(); i++)
        {
            lineData.push_back(Vertex{x[i], y[i]});
        }
    }

    Eigen::Vector3f colour()
    {
        return lineColour.colour;
    }

    RenderData loadDataToBuffers(double xMin, double xMax, double yMin, double yMax)
    {
        std::vector<Vertex> scaledVertices = scaleVector(lineData, xMin, xMax, yMin, yMax);
        float vertices[lineData.size() * 2];
        int j = 0;
        for (int i = 0; i < (int)(lineData.size() * 2); i += 2)
        {
            vertices[i] = (float)scaledVertices[j].x;
            vertices[i + 1] = (float)scaledVertices[j].y;

            j++;
        }

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        return RenderData{VAO, (int)lineData.size(), colour()};
    }

private:
    // Colours/shades that can be chosen for lines (other colours could be reserved for axes, grid, etc)
    Colour validColours[NUM_COLOURS] = {
        Colour::WHITE,
        Colour::RED,
        Colour::GREEN,
        Colour::BLUE};

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
            double shiftedX = (vert.x + xAbsMinValue) / xShiftedMax - 0.5;
            double shiftedY = (vert.y + yAbsMinValue) / yShiftedMax - 0.5;
            Vertex scaledVertex{shiftedX, shiftedY};
            scaledVector.push_back(scaledVertex);
        }

        return scaledVector;
    }
};
