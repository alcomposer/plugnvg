//
// Created by alexmitchell on 16/02/24.
//
#include "EditorConnection.h"
#include "EditorNode.h"

EditorConnection::EditorConnection(Component *node) :  node(node)
{
    static int con = 0;
    setName("connection " + String(con++));
    setAlwaysOnTop(true);
}

void EditorConnection::renderNVG(NVGcontext *nvg)
{
    if (!node)
        return;
    auto startPos = node->getScreenBounds().toFloat().getCentre() - getTopLevelComponent()->getScreenPosition().toFloat();
    Point<float> endPos;
    if (endNode)
        endPos = endNode->getScreenBounds().toFloat().getCentre() - getTopLevelComponent()->getScreenPosition().toFloat();
    else
        endPos = Desktop::getMousePosition().toFloat() - getTopLevelComponent()->getScreenPosition().toFloat();

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
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        Point<int> cPoint1 = Point<int>(startPos.x, ((endPos.y - startPos.y) * 0.25f) + startPos.y);
        Point<int> cPoint2 = Point<int>(endPos.x, ((endPos.y - startPos.y) * 0.75f) + startPos.y);
        nvgBezierTo(nvg, cPoint1.x, cPoint1.y, cPoint2.x, cPoint2.y, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(40, 40, 40, 255));
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 4.0f);
//        nvgClosePath(nvg);
        nvgStroke(nvg);



        // inner line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, NVG_LINE_DASHED);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgBezierTo(nvg, cPoint1.x, cPoint1.y, cPoint2.x, cPoint2.y, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
        nvgStrokeWidth(nvg, 1.5f);
        nvgLineCap(nvg, NVG_ROUND);
//        nvgClosePath(nvg);
        nvgStroke(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);



    }
}