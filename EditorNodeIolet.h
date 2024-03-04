//
// Created by alexmitchell on 16/02/24.
//
#pragma once
#include "NVGComponent.h"

class EditorNode;
class EditorConnection;
class EditorNodeIolet : public NVGComponent
{
public:
    enum Iolet {Inlet, Outlet};

    EditorNodeIolet(EditorNode* parentNode, int index, Iolet type = Iolet::Outlet);

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
    void renderNVG(NVGWrapper * nvgWrapper) override
    {
        return;

        auto nvg = nvgWrapper->nvg;

        auto ioletColor = nvgRGB(66, 162, 200);

        auto parentPos = getParentComponent()->getPosition();
        //auto canvasPos = getParentComponent()->getParentComponent()->getPosition();
        auto finalPos = parentPos + pos;
        nvgSave(nvg);
        nvgBeginPath(nvg);
        nvgCircle(nvg, finalPos.x, finalPos.y, isActive ? 8 : 5);
        if (!isActive) {

            if (ioletType == Iolet::Outlet)
                nvgScissor(nvg, finalPos.x - 7, finalPos.y - 9, 14, 9);
            else
                nvgScissor(nvg, finalPos.x - 7, finalPos.y, 14, 9);

            nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
            nvgStrokeWidth(nvg, 2.0f);
            nvgStroke(nvg);
            nvgFillColor(nvg, ioletColor); // Red color
            nvgFill(nvg);

        } else {

            nvgFillColor(nvg, ioletColor); // Red color
            nvgFill(nvg);
            nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
            nvgStrokeWidth(nvg, 2.0f);
            nvgStroke(nvg);
        }
        nvgRestore(nvg);
    }


    WidgetType getType() override
    {
        return WidgetType::Iolet;
    }

    Iolet ioletType;
    Point<int> pos;
    int ioletIndex = 0;
    bool isActive = false;
    EditorNodeIolet* foundIolet = nullptr;

    SafePointer<EditorConnection> newConnection;
    bool addingNewConnection = false;
private:
    EditorNode* parentNode;
};

