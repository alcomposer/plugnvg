//
// Created by alexmitchell on 16/02/24.
//
#include "EditorConnection.h"
#include "EditorNode.h"

EditorConnection::EditorConnection(Component *node) : node(node)
{
    static int con = 0;
    setName("connection " + String(con++));
}

void EditorConnection::renderNVG(NVGcontext *nvg)
{
    auto startPos = node->getScreenPosition() - getTopLevelComponent()->getScreenPosition();
    Point<int> endPos;
    if (endNode)
        endPos = endNode->getScreenPosition() - getTopLevelComponent()->getScreenPosition();
    else
        endPos = Desktop::getMousePosition() - getTopLevelComponent()->getScreenPosition();

    if (isStraight) {
        // outer line
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgLineTo(nvg, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(10, 10, 10, 255));
        nvgStrokeWidth(nvg, 3.0f);
        nvgStroke(nvg);
        nvgClosePath(nvg);

        // inner line
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgLineTo(nvg, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
        nvgStrokeWidth(nvg, 2.0f);
        nvgStroke(nvg);
        nvgClosePath(nvg);
    } else {
        // outer line
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        Point<int> cPoint1 = Point<int>(startPos.x, ((endPos.y - startPos.y) * 0.25f) + startPos.y);
        Point<int> cPoint2 = Point<int>(endPos.x, ((endPos.y - startPos.y) * 0.75f) + startPos.y);
        nvgBezierTo(nvg, cPoint1.x, cPoint1.y, cPoint2.x, cPoint2.y, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(10, 10, 10, 255));
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 4.0f);
        nvgStroke(nvg);
        nvgClosePath(nvg);

        // inner line
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgBezierTo(nvg, cPoint1.x, cPoint1.y, cPoint2.x, cPoint2.y, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
        nvgStrokeWidth(nvg, 1.5f);
        nvgLineCap(nvg, NVG_ROUND);
        nvgStroke(nvg);
        nvgClosePath(nvg);
    }
}