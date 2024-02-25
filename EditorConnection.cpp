//
// Created by alexmitchell on 16/02/24.
//
#include "EditorConnection.h"
#include "EditorNode.h"

EditorConnection::EditorConnection(Component *node) :  node(node)
{
    static int con = 0;
    setName("connection " + String(con++));
    //setAlwaysOnTop(true);
}

void EditorConnection::renderNVG(NVGcontext *nvg)
{
    if (!node)
        return;

    auto mouse = Desktop::getInstance().getMainMouseSource().getScreenPosition();
    std::cout << "mouse is: " << mouse.toString() << std::endl;
    auto startPos = node->getScreenBounds().toFloat().getCentre() - getTopLevelComponent()->getScreenPosition().toFloat();
    Point<float> endPos;
    if (endNode)
        endPos = endNode->getScreenBounds().toFloat().getCentre() - getTopLevelComponent()->getScreenPosition().toFloat();
    else
        endPos = mouse - getTopLevelComponent()->getScreenPosition().toFloat();

    if (isStraight) {
        // outer line
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgLineTo(nvg, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(50, 50, 50, 255));
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 25.0f);
        nvgStroke(nvg);

        // inner line
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgLineTo(nvg, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
        nvgLineStyle(nvg, NVG_LINE_DASHED);
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 2.0f);
        nvgStroke(nvg);

    } else {
        // outer line

        const auto cPoint1 = Point<float>(startPos.x, ((endPos.y - startPos.y) * 0.25f) + startPos.y);
        const auto cPoint2 = Point<float>(endPos.x, ((endPos.y - startPos.y) * 0.25f) + startPos.y);


        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgBezierTo(nvg, cPoint1.x, cPoint1.y, cPoint2.x, cPoint2.y, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(50, 50, 50, 255));
      //  nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 25.0f);
        nvgStroke(nvg);

        // inner line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, NVG_LINE_DASHED);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgBezierTo(nvg, cPoint1.x, cPoint1.y, cPoint2.x, cPoint2.y, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 255));
        nvgStrokeWidth(nvg, 20.0f);
        nvgLineJoin(nvg, NVG_BEVEL);
        //nvgLineCap(nvg, NVG_ROUND);
        nvgStroke(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);

        nvgBeginPath(nvg);
        nvgFillColor(nvg, nvgRGB(0,255,0));
        nvgCircle(nvg, cPoint1.x, cPoint1.y, 5);
        nvgCircle(nvg, cPoint2.x, cPoint2.y, 5);
        nvgFill(nvg);


    }
}