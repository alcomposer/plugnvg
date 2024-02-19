//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include <JuceHeader.h>
#include "libraries/nanovg-dev/src/nanovg.h"

class NVGComponent : public Component
{
public:
    enum WidgetType {None = 0, Connection = 1, Node = 2, Iolet = 4};

    NVGComponent(){}

    virtual void renderNVG(NVGcontext* nvg) = 0;

    void render(NVGcontext* nvg)
    {
        renderNVG(nvg);
    }

    virtual WidgetType getType() { return None; }
};