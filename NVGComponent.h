//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include <JuceHeader.h>
#include <nanovg.h>

class NVGComponent : public Component
{
public:
    NVGComponent(){}

    virtual void renderNVG(NVGcontext* nvg) = 0;

    void render(NVGcontext* nvg)
    {
        renderNVG(nvg);
    }
};