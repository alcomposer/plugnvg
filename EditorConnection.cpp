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
        // calculate bezier path
        auto start = endPos;
        auto end = startPos;

        if ((node->ioletType == EditorNodeIolet::Outlet && !endNode) || (node->ioletType == EditorNodeIolet::Outlet)) {
            start = startPos;
            end = endPos;
        }

        float const width = std::max(start.x, end.x) - std::min(start.x, end.x);
        float const height = std::max(start.y, end.y) - std::min(start.y, end.y);

        float const min = std::min<float>(width, height);
        float const max = std::max<float>(width, height);

        float const maxShiftY = 40.f;
        float const maxShiftX = 20.f;

        float const shiftY = std::min<float>(maxShiftY, max * 0.5);

        float const shiftX = ((start.y >= end.y) ? std::min<float>(maxShiftX, min * 0.5) : 0.f) * ((start.x < end.x) ? -1. : 1.);

        Point<float> const cp1 { start.x - shiftX, start.y + shiftY };
        Point<float> const cp2 { end.x + shiftX, end.y - shiftY };

        // semi-transparent background line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgMoveTo(nvg, start.x, start.y);
        nvgBezierTo(nvg, cp1.x, cp1.y, cp2.x, cp2.y, end.x, end.y);
        nvgStrokeColor(nvg, nvgRGBA(100, 100, 100, 200));
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 5.5f);
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
        nvgStrokeColor(nvg, nvgRGBA(220, 220, 220, 255));
        nvgStrokeWidth(nvg, 2.5f);
        nvgStroke(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);

//#define DEBUG_CON
#ifdef DEBUG_CON
        nvgStrokeWidth(nvg, 1.0f);
        nvgStrokeColor(nvg, nvgRGB(0,255,0));
        nvgBeginPath(nvg);
        nvgMoveTo(nvg, start.x, start.y);
        nvgLineTo(nvg, cp1.x, cp1.y);
        nvgStroke(nvg);

        nvgBeginPath(nvg);
        nvgMoveTo(nvg, end.x, end.y);
        nvgLineTo(nvg, cp2.x, cp2.y);
        nvgStroke(nvg);

        nvgBeginPath(nvg);
        nvgFillColor(nvg, nvgRGB(0,255,0));
        nvgCircle(nvg, cp1.x, cp1.y, 5);
        nvgFill(nvg);

        nvgBeginPath(nvg);
        nvgCircle(nvg, cp2.x, cp2.y, 5);
        nvgFill(nvg);
#endif

    }
}