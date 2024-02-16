//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include "EditorNodeIolet.h"

class EditorNode : public NVGComponent
{
public:
    EditorNode(Point<int> pos) : pos(pos)
    {
        setInterceptsMouseClicks(true, false);
        static int nodeNumber = 0;
        setName("node " + String(nodeNumber++));
        setSize(100, 30);
        setCentrePosition(pos);
        auto ioletPos = 5;
        for (int i = 0; i < 3; i++) {
            auto iolet = new EditorNodeIolet();
            iolets.add(iolet);
            addAndMakeVisible(iolet);
            iolet->setCentrePosition(ioletPos, 0);
            iolet->pos.x = ioletPos;
            ioletPos += 15;
        }
    }

    void mouseEnter(MouseEvent const& e) override
    {
        isHover = true;
    }

    void mouseExit(MouseEvent const& e) override
    {
        isHover = false;
    }

    void mouseDrag(MouseEvent const& e) override
    {
        //if (lock.tryEnter())
        if (e.originalComponent != this)
            return;
        pos = e.getScreenPosition() - getParentComponent()->getScreenPosition();
        //std::cout << "pos: " << pos.toString() << std::endl;
        setCentrePosition(pos);
    }

    void renderNVG(NVGcontext* nvg) override
    {
        auto parentLeft = getParentComponent()->getBounds().getTopLeft();
        auto b = getBounds().translated(parentLeft.getX(), parentLeft.getY());
        nvgBeginPath(nvg);
        auto cSelect = nvgRGBf(.3, .3, .3);
        auto cDefault = nvgRGBf(.2, .2, .2);
        auto colour = isHover ? cSelect : cDefault;
        nvgFillColor(nvg, colour);
        nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 5);
        nvgFill(nvg);
        nvgStrokeColor(nvg, nvgRGBf(.9, .9, .9));
        nvgStrokeWidth(nvg, 1.f);
        nvgStroke(nvg);
        nvgClosePath(nvg);
    }
private:
    Point<int> pos;
    bool isHover = false;
    OwnedArray<EditorNodeIolet> iolets;
};