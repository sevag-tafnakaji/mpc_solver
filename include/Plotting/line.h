#ifndef LINE_H
#define LINE_H
#include "colour.h"
#include "utils.h"

#define NUM_COLOURS 4

/**
 * TODO: Add following options:
 * Varied forms of dashed lines
 * lines with (textured) points on given vertices?
 */
class Line
{
public:
    std::vector<Vertex> lineData;
    Colour lineColour;

    Line(std::vector<Vertex> _lineData, Colour _lineColour) : lineData{_lineData}, lineColour{_lineColour} {}

    Line(std::vector<Vertex> _lineData)
    {
        lineData = _lineData;
    }

    Line(std::vector<double> x, std::vector<double> y, Eigen::Vector3f _lineColour)
    {
        setData(x, y);
        lineColour.setColour(_lineColour);
    }

    Line(std::vector<double> x, std::vector<double> y, std::string _lineColour)
    {
        setData(x, y);
        lineColour.setColour(_lineColour);
    }

    Line(int numLines)
    {
        lineColour.setColour(validColours[numLines]);
    }

    void setColour(std::string _colour)
    {
        lineColour.setColour(_colour);
    }

    void setColour(float r, float g, float b)
    {
        lineColour.setColour(r, g, b);
    }

    void setColour(Eigen::Vector3f _colour)
    {
        lineColour.setColour(_colour[0], _colour[1], _colour[2]);
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

    RenderData loadDataToBuffers(double xMin, double xMax, double yMin, double yMax, GLenum mode)
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

        return RenderData{VAO, (int)lineData.size(), colour(), mode};
    }

private:
    // Colours/shades that can be chosen for lines (other colours could be reserved for axes, grid, etc)
    PredefinedColour validColours[NUM_COLOURS] = {
        PredefinedColour::ORANGE,
        PredefinedColour::RED,
        PredefinedColour::GREEN,
        PredefinedColour::BLUE};
};

#endif
