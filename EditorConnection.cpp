//
// Created by alexmitchell on 16/02/24.
//
#include "EditorConnection.h"
#include "EditorNode.h"
#include "EditorNodeIolet.h"
#include "EditorNodeCanvas.h"

EditorConnection::EditorConnection(EditorNodeIolet *node) :  node(node)
{
    static int con = 0;
    setName("connection " + String(con++));
    //setAlwaysOnTop(true);
}

EditorConnection::EditorConnection(EditorNodeIolet* startNode, EditorNodeIolet* endNode) : node(startNode), endNode(endNode)
{
}

void EditorConnection::renderNVG(NVGWrapper *nvgWrapper)
{
    if (!node)
        return;

    auto nvg = nvgWrapper->nvg;
    auto cnv = findParentComponentOfClass<EditorNodeCanvas>();

    auto mouse = Desktop::getInstance().getMainMouseSource().getScreenPosition();
    auto startPos = cnv->getLocalPoint(nullptr, node->getScreenBounds().toFloat().getCentre());// - getTopLevelComponent()->getScreenPosition().toFloat();
    Point<float> endPos;
    if (endNode)
        endPos = cnv->getLocalPoint(nullptr, endNode->getScreenBounds().toFloat().getCentre());// - getTopLevelComponent()->getScreenPosition().toFloat();
    else
        endPos = cnv->getLocalPoint(nullptr, mouse);

    if (isStraight) {
        // outer line
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, startPos.x, startPos.y);
        nvgLineTo(nvg, endPos.x, endPos.y);
        nvgStrokeColor(nvg, nvgRGBA(50, 50, 50, 255));
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 4.0f);
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

        nvgLineStyle(nvg, NVG_LINE_SOLID);

    } else {
        // outer line

        const auto cPoint1 = Point<float>(startPos.x, ((endPos.y - startPos.y) * 0.75f) + startPos.y);
        const auto cPoint2 = Point<float>(endPos.x, ((endPos.y - startPos.y) * 0.25f) + startPos.y);

        auto cp1 = cPoint1;
        auto cp2 = cPoint2;
        auto start = startPos;
        auto end = endPos;

        if (node->ioletType == EditorNodeIolet::Outlet || !endNode) {
            start = endPos;
            end = startPos;
            cp1 = cPoint2;
            cp2 = cPoint1;
        }

        // semi-transparent background line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgMoveTo(nvg, start.x, start.y);
        nvgBezierTo(nvg, cp1.x, cp1.y, cp2.x, cp2.y, end.x, end.y);
        nvgStrokeColor(nvg, nvgRGBA(45, 45, 45, 100));
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 6.0f);
        nvgStroke(nvg);
/*
        // solid background line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgMoveTo(nvg, start.x, start.y);
        nvgBezierTo(nvg, cp1.x, cp1.y, cp2.x, cp2.y, end.x, end.y);
        nvgStrokeColor(nvg, nvgRGBA(55, 55, 55, 255));
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 3.5f);
        nvgStroke(nvg);
*/
        // inner line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, cableType == CableType::Signal ? NVG_LINE_DASHED : NVG_LINE_SOLID);
        nvgMoveTo(nvg, start.x, start.y);
        nvgBezierTo(nvg, cp1.x, cp1.y, cp2.x, cp2.y, end.x, end.y);
        nvgStrokeColor(nvg, nvgRGBA(250, 250, 250, 255));
        nvgStrokeWidth(nvg, 2.0f);
        nvgStroke(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);

//#define DEBUG_CON
#ifdef DEBUG_CON
        nvgBeginPath(nvg);
        nvgFillColor(nvg, nvgRGB(0,255,0));
        nvgCircle(nvg, cPoint1.x, cPoint1.y, 5);
        nvgCircle(nvg, cPoint2.x, cPoint2.y, 5);
        nvgFill(nvg);
#endif

    }
}