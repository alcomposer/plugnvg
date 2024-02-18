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
    EditorNodeCanvas(NVGComponent* parent) : NVGComponent(parent)
    {
        setName("node canvas");

        Random random;
        Range range = Range<int>(4000, 6000);

#if (TEST_1000 == 1)
        for (int i = 0; i < 1000; i++) {
            auto pos = Point<int>(random.nextInt(range), random.nextInt(range));
            auto node = new EditorNode(this, pos);
            //node->setCentrePosition(pos);
            nodes.add(node);
            addAndMakeVisible(node);
            addNVGComponent(node);
        }
#endif
    }

    void mouseDown(MouseEvent const& e) override
    {
        if (e.mods.isMiddleButtonDown()) {
            currentPos = getPosition();
        } else if (e.mods.isLeftButtonDown()){
            isPressed = true;
            auto pos = e.getPosition();
            auto node = new EditorNode(this, pos);
            //node->setCentrePosition(pos);
            nodes.add(node);
            addAndMakeVisible(node);
            addNVGComponent(node);
        }
    }
    void mouseUp(MouseEvent const& e) override
    {
        isPressed = false;
        setMouseCursor(MouseCursor::NormalCursor);
    }

    void mouseDrag(MouseEvent const& e) override
    {
        if (e.mods.isMiddleButtonDown()) {
            setMouseCursor(MouseCursor::DraggingHandCursor);
            auto delta = currentPos + e.getOffsetFromDragStart();
            setTopLeftPosition(delta);
        }
    }

    void renderNVG(NVGcontext* nvg) override
    {
        auto b = getBounds();
        //std::cout << "canvas bounds= " << b.toString() << std::endl;
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

