//
// Created by alexmitchell on 16/02/24.
//
#include "EditorConnection.h"
#include "EditorNode.h"
#include "EditorNodeIolet.h"
#include "EditorNodeCanvas.h"

EditorConnection::EditorConnection(EditorNodeIolet *startNode) :  node(startNode)
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

    auto const defaultColour = nvgRGB(220, 220, 220);
    auto const selectedColour = nvgRGB(66, 162, 200);
    auto const outlineColour = nvgRGBAf(.4, .4, .4, .4);//nvgRGBA(100, 100, 100, 200)

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
        start_ = endPos;
        end_ = startPos;

        if ((node->ioletType == EditorNodeIolet::Outlet && !endNode) || (node->ioletType == EditorNodeIolet::Outlet)) {
            start_ = startPos;
            end_ = endPos;
        }

        float const width = std::max(start_.x, end_.x) - std::min(start_.x, end_.x);
        float const height = std::max(start_.y, end_.y) - std::min(start_.y, end_.y);

        Point<float> const pos = Point<float>(jmin(start_.x, end_.x), jmin(start_.y, end_.y));

        float const min = std::min<float>(width, height);
        float const max = std::max<float>(width, height);

        float const shiftY = std::min<float>(maxShiftY, max * 0.5);

        float const shiftX = ((start_.y >= end_.y) ? std::min<float>(maxShiftX, min * 0.5) : 0.f) * ((start_.x < end_.x) ? -1. : 1.);

        cp1 = Point<float>(start_.x - shiftX, start_.y + shiftY);
        cp2 = Point<float>(end_.x + shiftX, end_.y - shiftY);

        auto bounds = Rectangle<int>(pos.x, pos.y, width, height).expanded(14);
        setBounds(bounds);

        // semi-transparent background line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, NVG_LINE_SOLID);
        nvgMoveTo(nvg, start_.x, start_.y);
        nvgBezierTo(nvg, cp1.x, cp1.y, cp2.x, cp2.y, end_.x, end_.y);
        nvgStrokeColor(nvg, outlineColour);
        nvgLineCap(nvg, NVG_ROUND);
        nvgStrokeWidth(nvg, 5.5f);
        nvgStroke(nvg);

        // inner line
        nvgBeginPath(nvg);
        nvgLineStyle(nvg, cableType == CableType::Signal ? NVG_LINE_DASHED : NVG_LINE_SOLID);
        nvgMoveTo(nvg, start_.x, start_.y);
        nvgBezierTo(nvg, cp1.x, cp1.y, cp2.x, cp2.y, end_.x, end_.y);
        nvgStrokeColor(nvg, isMouseOver(false) ? selectedColour : defaultColour);
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