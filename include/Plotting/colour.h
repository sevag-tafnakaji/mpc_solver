#include <iostream>

#include <Eigen/Eigen>

enum Colour
{
    WHITE,
    BLACK,
    ORANGE,
    RED,
    GREEN,
    BLUE
};

class LineColour
{
public:
    Eigen::Vector3f colour;

    LineColour(std::string _colour)
    {
        setLineColour(_colour);
    };

    LineColour(float r, float g, float b)
    {
        setLineColour(r, g, b);
    }

    LineColour(Eigen::Vector3f _colour)
    {
        setLineColour(_colour);
    }

    LineColour()
    {
        colour = BLACK;
    };

    void setLineColour(std::string _colour)
    {
        Colour colourEnum = stringToColourEnum[_colour];
        colour = getColour(colourEnum);
    }

    void setLineColour(Colour _colour)
    {
        colour = getColour(_colour);
    }

    void setLineColour(float r, float g, float b)
    {
        if (withinBounds(r) && withinBounds(g) && withinBounds(b))
            colour = {r, g, b};
        else
        {
            std::cout << "Provided colour values that not between 0 and 1. Setting colour to black" << std::endl;
            colour = BLACK;
        }
    }

    void setLineColour(Eigen::Vector3f _colour)
    {
        colour = _colour;
    }

private:
    // All of the pre-defined colours
    std::map<std::string, Colour> stringToColourEnum{
        {"WHITE", Colour::WHITE},
        {"BLACK", Colour::BLACK},
        {"ORANGE", Colour::ORANGE},
        {"RED", Colour::RED},
        {"GREEN", Colour::GREEN},
        {"BLUE", Colour::BLUE},
    };

    Eigen::Vector3f WHITE{1.0f, 1.0f, 1.0f};
    Eigen::Vector3f BLACK{0.0f, 0.0f, 0.0f};
    Eigen::Vector3f ORANGE{1.0f, 0.5f, 0.2f};
    Eigen::Vector3f RED{1.0f, 0.0f, 0.0f};
    Eigen::Vector3f GREEN{0.0f, 1.0f, 0.0f};
    Eigen::Vector3f BLUE{0.0f, 0.0f, 1.0f};

    Eigen::Vector3f getColour(Colour _colour)
    {
        switch (_colour)
        {
        case (Colour::WHITE):
            return WHITE;
            break;
        case (Colour::BLACK):
            return BLACK;
            break;
        case (Colour::ORANGE):
            return ORANGE;
            break;
        case (Colour::RED):
            return RED;
            break;
        case (Colour::GREEN):
            return GREEN;
            break;
        case (Colour::BLUE):
            return BLUE;
            break;
        }
    }

    bool withinBounds(float value)
    {
        return (value >= 0.0f && value <= 1.0f);
    }
};
