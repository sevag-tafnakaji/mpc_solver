#include "Plotting/plotter.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

Plotter::Plotter()
{
    this->init();
}

std::vector<double> scale_vector(std::vector<double> vect)
{
    // scale vector such that all new values would be [-0.5, 0.5]
    double min_value = *std::min_element(vect.begin(), vect.end());
    double max_value = *std::max_element(vect.begin(), vect.end());

    double abs_min_value = abs(min_value);

    double shifted_max = max_value + abs_min_value;

    std::vector<double> scaled_vector;

    for (double elem : vect)
    {
        double shifted_elem = (elem + abs_min_value) / shifted_max - 0.5;
        scaled_vector.push_back(shifted_elem);
    }

    return scaled_vector;
}

void Plotter::draw(std::vector<double> x, std::vector<double> y)
{
    // TODO: Ensure that x and y are of the same size

    float vertices[x.size() * 2];

    std::vector<double> new_x = scale_vector(x);
    std::vector<double> new_y = scale_vector(y);

    int j = 0;
    // Scale values to range from -0.5 to 0.5 (horizontally and vertically)
    for (int i = 0; i < x.size() * 2; i += 2)
    {
        vertices[i] = new_x[j];
        vertices[i + 1] = new_y[j];
        j++;
    }

    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ResourceManager::GetShader("plotter").Use();

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, x.size());

    glBindVertexArray(0);
}

void Plotter::update()
{
}

void Plotter::init()
{
    ResourceManager::LoadShader("../resources/shaders/shader.vs", "../resources/shaders/shader.fs", nullptr, "plotter");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, // left
        0.0f, 0.5f,   // top
        0.5f, -0.5f,  // right
    };

    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
