#ifndef PLOTTER_H
#define PLOTTER_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <limits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "resource_manager.h"
#include "line.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

#define NUM_X_TICKS 9
#define NUM_Y_TICKS 7
#define TICK_OFFSET 0.025f
#define TEXT_OFFSET 10

struct Character
{
    unsigned int TextureID;  // ID handle of the glyph texture
    Eigen::Vector2i Size;    // Size of glyph
    Eigen::Vector2i Bearing; // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

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

    std::map<char, Character> characters;

private:
    // settings
    int SCR_WIDTH = 800;
    int SCR_HEIGHT = 600;

    std::vector<Line> plotLines;

    std::vector<Vertex> xTickPositions;
    std::vector<Vertex> yTickPositions;

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

    // Text gets special VBO + VAO
    unsigned int textVBO, textVAO;

    void updateBuffers(unsigned int buffer, int numVertices, Eigen::Vector3f colour, GLenum mode);
    void updateBuffers(RenderData data);

    void extractMinMaxValues();

    void loadDataToBuffers();

    void init();

    void initFont();

    void plotAxes();

    void renderText(std::string text, float x, float y, float scale, Colour colour);

    void renderTickLabels();

    void plotAxesTicks();
};

#endif
