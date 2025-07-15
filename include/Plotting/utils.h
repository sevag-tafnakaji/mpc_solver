#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <random>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Eigen/Eigen>

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
    GLenum mode;
};

double random_zero_to_one();

double scalePoint(double raw, double minValue, double maxValue);

double scaleBackPoint(double scaled, double scaleEdgeValue, double minValue, double maxValue);

std::vector<Vertex> scaleVector(std::vector<Vertex> vect, double xMinValue, double xMaxValue, double yMinValue, double yMaxValue);

bool comp_x(const Vertex &a, const Vertex &b);

bool comp_y(const Vertex &a, const Vertex &b);

#endif
