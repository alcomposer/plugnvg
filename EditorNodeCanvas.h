//
// Created by alexmitchell on 16/02/24.
//

#pragma once
#include <JuceHeader.h>

#include "NVGComponent.h"
#include "EditorNode.h"
#include "EditorConnection.h"
#include "NVGLasso.h"

class EditorNodeCanvas : public NVGComponent, public LassoSource<EditorNode*>, public ChangeListener
{
public:
    EditorNodeCanvas();

    void mouseDown(MouseEvent const& e) override
    {
        if (e.mods.isMiddleButtonDown()) {
            currentPos = getPosition();
        } else if (e.mods.isLeftButtonDown() && e.getNumberOfClicks() == 2) {
            isPressed = true;
            auto pos = e.getPosition();
            auto node = new EditorNode(pos);
            //node->setCentrePosition(pos);
            nodes.add(node);
            addAndMakeVisible(node);
            setSingleSelected(nullptr);
        } else if (e.mods.isLeftButtonDown()) {
            lasso.beginLasso(e.getEventRelativeTo(this), this);
            setSingleSelected(nullptr);
        }
    }

    void mouseDrag(MouseEvent const& e) override;

    void mouseUp(MouseEvent const& e) override
    {
        isPressed = false;
        setMouseCursor(MouseCursor::NormalCursor);
        lasso.endLasso();
    }

    void findLassoItemsInArea (Array<EditorNode*>& itemsFound,
                                       const Rectangle<int>& area) override
    {
        //std::cout << "lasso bounds: " << area.toString() << std::endl;
        for (auto* node : nodes) {
            auto selB = node->getBounds();
            //std::cout << "node bounds: " << selB.toString() << std::endl;
            if (area.intersects(selB)) {
                node->isSelected = true;
                itemsFound.add(node);
            } else  {
                node->isSelected = false;

            }
        }
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        return;
        if (source == &selectedComponents) {
            for (auto node : nodes) {
                node->getProperties().set("is_selected", var(false));
            }
            for (auto node : selectedComponents) {
                node->getProperties().set("is_selected", var(true));
            }
        }
    }

    SelectedItemSet<EditorNode*>& getLassoSelection() override
    {
        return selectedComponents;
    }

    void renderNVG(NVGcontext* nvg) override
    {
        // instead of locking (which would never work anyway) we make Juce update the position of the canvas
        // from the openGL callback. This way it will be the same position during the entire opengl render call
        TRACE_COMPONENT();
        setTopLeftPosition(delta);
        //return;
        auto b = Rectangle<int>(delta.x, delta.y, canvasSize, canvasSize);
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

    void removeConnection();

    void setSingleSelected(EditorNode* nodeToSelect)
    {
        if (nodeToSelect != singleSelected) {
            if (singleSelected)
                singleSelected->isSelected = false;

            singleSelected = nodeToSelect;

            if (singleSelected)
                singleSelected->isSelected = true;
        }
    }

    // TODO: this needs to be a safepointer!
    // Needs to be allocated before object and connection so they can deselect themselves in the destructor
    SelectedItemSet<EditorNode*> selectedComponents;

private:


    bool isPressed = false;
    OwnedArray<EditorNode> nodes;
    OwnedArray<EditorConnection> cons;
    Point<int> currentPos;
    Point<int> mouseDownPos;
    Point<int> delta;

    EditorNode* singleSelected = nullptr;

    NVGLasso lasso;

    static constexpr int canvasSize = 200000;
    static constexpr int halfSize = canvasSize / 2;
};

