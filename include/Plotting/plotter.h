#ifndef PLOTTER_H
#define PLOTTER_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <limits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "resource_manager.h"
#include "line.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

class Plotter
{
public:
    Plotter();
    ~Plotter();

    virtual void plot(std::vector<double> x, std::vector<double> y);
    virtual void plot(std::vector<double> x, std::vector<double> y, std::string colour);
    virtual void plot(std::vector<double> x, std::vector<double> y, Eigen::Vector3f colour);

    // Main loop that renders all active VAOs
    virtual void render();

private:
    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    std::vector<Line> plotLines;

    double xMin{std::numeric_limits<double>::max()};
    double yMin{std::numeric_limits<double>::max()};
    double xMax{std::numeric_limits<double>::min()};
    double yMax{std::numeric_limits<double>::min()};

    // Single window for now (vector for multiple in the future?)
    GLFWwindow *window;

    // All VAOs (data, axes, grids, etc) + data related to drawing lines as desired
    std::vector<RenderData> VAOs;

    // only active VAOs + data related to drawing lines as desired
    std::vector<RenderData> activeVAOs;

    void updateBuffers(unsigned int buffer, int numVertices, Eigen::Vector3f colour);
    void updateBuffers(RenderData data);

    void extractMinMaxValues();

    void loadDataToBuffers();

    void init();

    void plotAxes();
};

#endif
