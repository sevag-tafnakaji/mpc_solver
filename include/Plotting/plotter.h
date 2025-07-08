#ifndef PLOTTER_H
#define PLOTTER_H

#include "resource_manager.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

class Plotter
{
public:
    Plotter();
    ~Plotter();

    virtual void plot(std::vector<double> x, std::vector<double> y);

    // Main loop that renders all active VAOs
    virtual void render();

private:
    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    // Single window for now (vector for multiple in the future?)
    GLFWwindow *window;

    // All VAOs (data, axes, grids, etc) + num vertices
    std::vector<std::pair<unsigned int, int>> VAOs;

    // only active VAOs + num vertices
    std::vector<std::pair<unsigned int, int>> activeVAOs;

    void updateBuffers(unsigned int buffer, int numVertices);

    void init();

    void plotAxes();
};

#endif
