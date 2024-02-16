//
// Created by alexmitchell on 16/02/24.
//

#pragma once
#include <JuceHeader.h>
#include "NVGComponent.h"
#include "EditorNode.h"
#include "EditorConnection.h"

class EditorNodeCanvas : public NVGComponent
{
public:
    EditorNodeCanvas()
    {
        setName("node canvas");
    }

    void mouseDown(MouseEvent const& e) override
    {
        if (e.mods.isMiddleButtonDown()) {
            currentPos = e.getPosition();
        } else if (e.mods.isLeftButtonDown()){
            isPressed = true;
            auto pos = e.getPosition();
            auto node = new EditorNode(pos);
            //node->setCentrePosition(pos);
            nodes.add(node);
            addAndMakeVisible(node);
        }
    }
    void mouseUp(MouseEvent const& e) override
    {
        isPressed = false;
    }

    void mouseDrag(MouseEvent const& e) override
    {
        if (e.mods.isMiddleButtonDown()) {
            auto pos = e.getScreenPosition();
            auto delta = pos - currentPos;
            currentPos = pos;

            auto move = getPosition().translated(delta.getX(), delta.getY());
            setTopLeftPosition(move);
            //for (auto node : nodes) {
            //    auto move = node->getBounds().translated(delta.getX(), delta.getY());
            //    node->setBounds(move);
            //}
        }
    }

    void renderNVG(NVGcontext* nvg) override
    {
        auto b = getBounds();
        nvgBeginPath(nvg);
        auto defaultColor = nvgRGBf(.3, .3, .3);
        auto selectedColor = nvgRGBf(.2, .2, .2);
        auto finalColor = isPressed ? selectedColor : defaultColor;
        nvgFillColor(nvg, defaultColor);
        nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 10);
        nvgFill(nvg);
        nvgClosePath(nvg);
    }

    void addConnection(EditorConnection* newConnection)
    {
        cons.add(newConnection);
        addAndMakeVisible(newConnection);
    }

    void removeConnection()
    {
        cons.removeLast();
    }

private:
    bool isPressed = false;
    OwnedArray<EditorNode> nodes;
    OwnedArray<EditorConnection> cons;
    Point<int> currentPos;
    Point<int> mouseDownPos;
};

