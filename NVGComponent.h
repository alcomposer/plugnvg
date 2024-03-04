//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include <JuceHeader.h>
#include "libraries/nanovg-dev/src/nanovg.h"

struct NVGWrapper
{
    NVGcontext* nvg;
    Point<int> mousePosScreen;
    int interFont;
};

class NVGComponent : public Component
{
public:
    enum WidgetType {None = 0, Connection = 1, Node = 2, Iolet = 4};

    NVGComponent(){}

    virtual void renderNVG(NVGWrapper* nvg) = 0;

    virtual void resetNVG(NVGcontext* nvg) {};

    void render(NVGWrapper* nvg)
    {
        //TRACE_COMPONENT();
        renderNVG(nvg);
    }

    virtual WidgetType getType() { return None; }
};