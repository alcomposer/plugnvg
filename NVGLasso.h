//
// Created by alexmitchell on 22/02/24.
//

#pragma once
#include <JuceHeader.h>
#include "NVGComponent.h"

class NVGLassoDrawing : public NVGComponent
{
public:
    NVGLassoDrawing(){};

    void resized()
    {
        b = getBounds();
        std::cout << "bounds: " << b.toString() << std::endl;
    }

    void renderNVG(NVGWrapper *nvgWrapper) override {
        auto nvg = nvgWrapper->nvg;
        auto defaultColor = nvgRGBAf(.3f, .3f, 1.f, 0.2f);
        auto outlineColour = nvgRGBf(.3f, .3f, 1.f);
        nvgBeginPath(nvg);
        nvgFillColor(nvg, defaultColor);
        nvgRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight());
        nvgFill(nvg);
        nvgStrokeColor(nvg, outlineColour);
        nvgStrokeWidth(nvg, 1.0f);
        nvgStroke(nvg);
    }
private:
    Rectangle<int> b;
};

class NVGLasso : public LassoComponent<EditorNode*>
{
public:
    NVGLasso()
    {
        addAndMakeVisible(lassoDrawing);
        setAlwaysOnTop(true);
    };

    void resized()
    {
        lassoDrawing.setBounds(getBoundsInParent());
    }
private:
    NVGLassoDrawing lassoDrawing;


};