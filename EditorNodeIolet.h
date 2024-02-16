//
// Created by alexmitchell on 16/02/24.
//
#pragma once
#include "NVGComponent.h"
#include "EditorConnection.h"


class EditorNodeIolet : public NVGComponent
{
public:
    EditorNodeIolet()
    {
        static int number = 0;
        setName("iolet " + String(number++));
        setSize(10,10);
    }

    void mouseEnter(MouseEvent const& e) override
    {
        isActive = true;
    }

    void mouseDown(MouseEvent const& e) override;
    void mouseUp(MouseEvent const& e) override;

    void mouseDrag(MouseEvent const& e) override
    {
        if (auto iolet = dynamic_cast<EditorNodeIolet*>(getTopLevelComponent()->getComponentAt(e.getScreenPosition() - getTopLevelComponent()->getScreenPosition()))) {
            if (iolet == this)
                return;
            iolet->isActive = true;
            foundIolet = iolet;
        } else if (foundIolet) {
            foundIolet->isActive = false;
            foundIolet = nullptr;
        }
    }

    void mouseExit(MouseEvent const& e) override
    {
        isActive = false;
    }
    void renderNVG(NVGcontext * nvg) override
    {
        auto parentPos = getParentComponent()->getPosition();
        auto canvasPos = getParentComponent()->getParentComponent()->getPosition();
        auto finalPos = parentPos + canvasPos + pos;
        nvgBeginPath(nvg);
        nvgCircle(nvg, finalPos.x, finalPos.y, isActive ? 8 : 5);
        nvgFillColor(nvg, nvgRGBA(255, 0, 0, 255)); // Red color
        nvgFill(nvg);
    }

    Point<int> pos;
    bool isActive = false;
    EditorNodeIolet* foundIolet = nullptr;

private:
    EditorConnection* newConnection;
};

