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
        g.fillAll(Colours::red);
    }
};
