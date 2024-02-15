//
// Created by alexmitchell on 11/02/24.
//

#pragma once

#include <JuceHeader.h>

class TestBox : public Component
{
public:
    TestBox(){}
    ~TestBox(){}

    void paint(Graphics& g) override
    {
        std::cout << "painting testbox" << std::endl;
        g.setColour(col);
        g.fillRoundedRectangle(0.0f, 0.0f, getWidth(), getHeight(), 18);
    }

    void setBoxColour(Colour c)
    {
        if (col == c)
            return;
        col = c;
        repaint();
    }
private:
    Colour col = Colours::red;
};
