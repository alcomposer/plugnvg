//
// Created by alexmitchell on 16/02/24.
//

#pragma once
#include <JuceHeader.h>
#include "NVGComponent.h"


class EditorNode;
class EditorNodeIolet;
class EditorConnection : public NVGComponent
{
public:

    enum CableType {Data, Signal, MCSignal};

    EditorConnection(EditorNodeIolet* startNode);

    EditorConnection(EditorNodeIolet* startNode, EditorNodeIolet* endNode);

    void resized()
    {
        auto cnv = getParentComponent();
        if (isStraight) {
        } else {
            hitTestPath.clear();
            hitTestPath.startNewSubPath(getLocalPoint(cnv, start_));
            hitTestPath.cubicTo(getLocalPoint(cnv, cp1), getLocalPoint(cnv, cp2), getLocalPoint(cnv,end_));
        }
    }

    bool hitTest(int x, int y) override
    {
        if (ModifierKeys::getCurrentModifiers().isMiddleButtonDown())
            return false;

        Point<float> position = Point<float>(static_cast<float>(x), static_cast<float>(y));
        Point<float> nearestPoint;
        hitTestPath.getNearestPoint(position, nearestPoint);
        return nearestPoint.getDistanceFrom(position) < 3;
    }

    void paintOverChildren(Graphics& g) override
    {
#ifdef DEBUG_CONNECTION_OUTLINE
        g.setColour(Colours::green);
        g.drawRect(getLocalBounds(), 1.0f);
#endif
#ifdef DEBUG_CONNECTION_HITPATH
        g.setColour(Colours::green);
        g.strokePath(hitTestPath, PathStrokeType(3.0f));
#endif
    }

    void renderNVG(NVGWrapper* nvg) override;

    Component* endNode = nullptr;

private:
    EditorNodeIolet* node;
    bool isStraight = false;
    CableType cableType = Data;

    const float maxShiftY = 40.f;
    const float maxShiftX = 20.f;

    Point<float> cp1;
    Point<float> cp2;

    Point<float> start_;
    Point<float> end_;

    Path hitTestPath;

    Point<float> offsetHack = Point<float>(100000,100000);
};

