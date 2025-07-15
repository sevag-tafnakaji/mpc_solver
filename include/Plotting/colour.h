#ifndef COLOUR_H
#define COLOUR_H

#include <iostream>

#include <Eigen/Eigen>

enum PredefinedColour
{
    WHITE,
    BLACK,
    ORANGE,
    RED,
    GREEN,
    BLUE
};

class Colour
{
public:
    Eigen::Vector3f colour;

    Colour(std::string _colour)
    {
        setColour(_colour);
    };

    Colour(float r, float g, float b)
    {
        setColour(r, g, b);
    }

    Colour(Eigen::Vector3f _colour)
    {
        setColour(_colour);
    }

    Colour()
    {
        colour = BLACK;
    };

    void setColour(std::string _colour)
    {
        PredefinedColour colourEnum = stringToColourEnum[_colour];
        colour = getColour(colourEnum);
    }

    void setColour(PredefinedColour _colour)
    {
        colour = getColour(_colour);
    }

    void setColour(float r, float g, float b)
    {
        if (withinBounds(r) && withinBounds(g) && withinBounds(b))
            colour = {r, g, b};
        else
        {
            std::cout << "Provided colour values that not between 0 and 1. Setting colour to black" << std::endl;
            colour = BLACK;
        }
    }

    void setColour(Eigen::Vector3f _colour)
    {
        colour = _colour;
    }

private:
    // All of the pre-defined colours
    std::map<std::string, PredefinedColour> stringToColourEnum{
        {"WHITE", PredefinedColour::WHITE},
        {"BLACK", PredefinedColour::BLACK},
        {"ORANGE", PredefinedColour::ORANGE},
        {"RED", PredefinedColour::RED},
        {"GREEN", PredefinedColour::GREEN},
        {"BLUE", PredefinedColour::BLUE},
    };

    Eigen::Vector3f WHITE{1.0f, 1.0f, 1.0f};
    Eigen::Vector3f BLACK{0.0f, 0.0f, 0.0f};
    Eigen::Vector3f ORANGE{1.0f, 0.5f, 0.2f};
    Eigen::Vector3f RED{1.0f, 0.0f, 0.0f};
    Eigen::Vector3f GREEN{0.0f, 1.0f, 0.0f};
    Eigen::Vector3f BLUE{0.0f, 0.0f, 1.0f};

    Eigen::Vector3f getColour(PredefinedColour _colour)
    {
        switch (_colour)
        {
        case (PredefinedColour::WHITE):
            return WHITE;
            break;
        case (PredefinedColour::BLACK):
            return BLACK;
            break;
        case (PredefinedColour::ORANGE):
            return ORANGE;
            break;
        case (PredefinedColour::RED):
            return RED;
            break;
        case (PredefinedColour::GREEN):
            return GREEN;
            break;
        case (PredefinedColour::BLUE):
            return BLUE;
            break;
        default:
            return BLACK;
            break;
        }
    }

    bool withinBounds(float value)
    {
        return (value >= 0.0f && value <= 1.0f);
    }
};

#endif
