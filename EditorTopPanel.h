//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include "NVGComponent.h"

class EditorTopPanel : public NVGComponent
{
public:
    EditorTopPanel()
    {
        setName("top panel");
    }

    void renderNVG(NVGWrapper* nvgWrapper) override
    {
        auto nvg = nvgWrapper->nvg;
        auto b = getBounds();
        nvgBeginPath(nvg);
        nvgFillColor(nvg, nvgRGBAf(0, 0, 1, 1));
        nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 10);
        nvgFill(nvg);
        nvgClosePath(nvg);
    }

private:

};
