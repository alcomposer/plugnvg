//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include <JuceHeader.h>
#include "NVGComponent.h"
#include "EditorTopPanel.h"
#include "EditorNodeCanvas.h"

class Editor : public NVGComponent
{
public:
    Editor(NVGComponent* c) : NVGComponent(c)
    {
        setName("editor");
        nodeCanvas = std::make_unique<EditorNodeCanvas>(this);
        addAndMakeVisible(nodeCanvas.get());
        addNVGComponent(nodeCanvas.get());
        nodeCanvas->setSize(10000, 10000);
        nodeCanvas->setTopLeftPosition(-5000, -5000);
        topPanel = std::make_unique<EditorTopPanel>(this);
        addAndMakeVisible(topPanel.get());
        addNVGComponent(topPanel.get());
    }

    void renderNVG(NVGcontext* nvg) override
    {
        auto b = getBounds();
        nvgBeginPath(nvg);
        nvgFillColor(nvg, nvgRGBAf(1, 1, 0, 1));
        nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 10);
        nvgFill(nvg);
        nvgClosePath(nvg);
    }

    void resized() override
    {
        topPanel->setBounds(0, 0, getWidth(), 38);
        //nodeCanvas.setTopLeftPosition(5000, 5000);
    }

private:
    std::unique_ptr<EditorTopPanel> topPanel;
    std::unique_ptr<EditorNodeCanvas> nodeCanvas;
};