#ifndef PLOTTER_H
#define PLOTTER_H

#include "resource_manager.h"
#include <vector>

class Plotter
{
public:
    Plotter();

    unsigned int VAO;
    unsigned int EBO;

    virtual void draw(std::vector<double> x, std::vector<double> y);
    virtual void update();

private:
    void init();
};

#endif
