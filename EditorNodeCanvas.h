//
// Created by alexmitchell on 16/02/24.
//

#pragma once
#include <JuceHeader.h>

#include "NVGComponent.h"
#include "EditorNode.h"
#include "EditorConnection.h"
#include "NVGLasso.h"


class TestComp : public NVGComponent
{
public:
    TestComp()
    {
        setBounds(100000, 100000, 50, 50);
    }

    bool hitTest(int x, int y) override
    {
        std::cout << "x: " << x << " y: " << y << std::endl;
        mousePos = Point<int>(x, y);
        return true;
    }

    void paint(Graphics& g) override
    {
        g.fillAll(Colours::red);
    }

    void renderNVG(NVGWrapper* nvgWrapper) override
    {
        auto nvg = nvgWrapper->nvg;
        nvgBeginPath(nvg);
        nvgCircle(nvg, mousePos.x, mousePos.y, 50);
        nvgFillColor(nvg, nvgRGBf(.5, .2, .8));
        nvgFill(nvg);
    }

    Point<int> mousePos;

};

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

    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override
    {
        auto newCanvasScale = canvasScale + (wheel.deltaY * 0.1);
        canvasScale = jmax(0.21875, jmin(10.0, newCanvasScale));
        //std::cout << "canvas scale: " << canvasScale << std::endl;
        mousePos = e.getEventRelativeTo(getParentComponent()).getPosition();
        //delta = getLocalBounds().getPosition() + getLocalPoint(nullptr, e.getScreenPosition()) * canvasScale;
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

    void renderNVG(NVGWrapper* nvgWrapper) override
    {
        auto nvg = nvgWrapper->nvg;
        // instead of locking (which would never work anyway) we make Juce update the position of the canvas
        // from the openGL callback. This way it will be the same position during the entire opengl render call
        // this may not be the best idea, but it works for now.
        //TRACE_COMPONENT();
        setTopLeftPosition(delta - mousePos);

        auto b = Rectangle<int>(0, 0, canvasSize, canvasSize);

        // apply translation to the canvas nvg objects
        nvgSave(nvg);
        //auto mousePos = getParentComponent()->getLocalPoint(nullptr, nvgWrapper->mousePosScreen);
        //std::cout << "mouse pos:" << mousePos.toString() << std::endl;
        auto scaledMousePos = mousePos;
        nvgTranslate(nvg, scaledMousePos.x, scaledMousePos.y);
        nvgScale(nvg, canvasScale, canvasScale);
        nvgTranslate(nvg, delta.x - scaledMousePos.x, delta.y - scaledMousePos.y);

        auto bgColour = nvgRGBf(.15, .15, .15);

        nvgBeginPath(nvg);
        if (resetDots == true) {
            dots = nvgDotPattern(nvg, nvgRGBf(.4, .4, .4), bgColour);
            resetDots = false;
        }
        //auto grad = nvgRadialGradient(nvg, 100000, 100000, 10, 100, nvgRGBf(1.0, 1.0, 1.0), bgColour);
        nvgFillPaint(nvg, dots);
        nvgRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight());
        nvgFill(nvg);

        nvgBeginPath(nvg);
        auto pos = Point<int>(halfSize, halfSize);
        nvgMoveTo(nvg, pos.x, pos.y + 100000);
        nvgLineTo(nvg, pos.x, pos.y);
        nvgLineTo(nvg, pos.x + 100000, pos.y);

        // place solid line behind (to fake removeing grid points for now)
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgStrokeColor(nvg, bgColour);
        nvgStrokeWidth(nvg, 6.0f);
        nvgStroke(nvg);

        // draw 0,0 point lines
        nvgLineStyle(nvg, NVG_LINE_DASHED);
        nvgStrokeColor(nvg, nvgRGBf(1, 1, 1));
        nvgStrokeWidth(nvg, 1.0f);
        nvgStroke(nvg);

        nvgLineStyle(nvg, NVG_LINE_SOLID);
    }

    void resetNVG(NVGcontext* nvg) override
    {
        nvgRestore(nvg);
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
    float canvasScaleRaw = 1.0f;
    float canvasScale = 1.0f;
    Point<int> mousePos;

    EditorNode* singleSelected = nullptr;

    NVGLasso lasso;

    static constexpr int canvasSize = 200000;
    static constexpr int halfSize = canvasSize / 2;

    TestComp testComp;

    NVGpaint dots;
    bool resetDots = true;
};

