//
// Created by alexmitchell on 16/02/24.
//
#pragma once
#include "NVGComponent.h"
#include "EditorConnection.h"


class EditorNodeIolet : public NVGComponent
{
public:
    enum Iolet {Inlet, Outlet};

    Iolet ioletType;

    EditorNodeIolet(Iolet type = Iolet::Outlet) : ioletType(type)
    {
        static int number = 0;
        setName("iolet " + String(number++));
        setSize(20,20);
    }

    void mouseEnter(MouseEvent const& e) override
    {
        isActive = true;
    }

    void mouseDown(MouseEvent const& e) override;
    void mouseUp(MouseEvent const& e) override;
    void mouseDrag(MouseEvent const& e) override;

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
        if (!isActive) {
            nvgSave(nvg);
            if (ioletType == Iolet::Outlet)
                nvgScissor(nvg, finalPos.x - 7, finalPos.y - 9, 14, 9);
            else
                nvgScissor(nvg, finalPos.x - 7, finalPos.y, 14, 9);
            nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
            nvgStrokeWidth(nvg, 2.0f);
            nvgStroke(nvg);
            nvgFillColor(nvg, nvgRGBA(255, 0, 0, 255)); // Red color
            nvgFill(nvg);
            nvgRestore(nvg);
        } else {
            nvgFillColor(nvg, nvgRGBA(255, 0, 0, 255)); // Red color
            nvgFill(nvg);
        }
    }

    WidgetType getType() override
    {
        return WidgetType::Iolet;
    }

    Point<int> pos;
    bool isActive = false;
    EditorNodeIolet* foundIolet = nullptr;

private:
    SafePointer<EditorConnection> newConnection;
};

