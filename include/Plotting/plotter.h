#ifndef PLOTTER_H
#define PLOTTER_H

#include "resource_manager.h"

class Plotter
{
public:
    Plotter();

    unsigned int VAO;
    unsigned int EBO;

    virtual void draw();
    virtual void update();

private:
    void init();
};

#endif
