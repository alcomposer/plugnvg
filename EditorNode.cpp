//
// Created by alexmitchell on 16/02/24.
//
#include "EditorNode.h"
#include "EditorNodeCanvas.h"
#include "GraphStateUpdater.h"

void EditorNode::mouseDown(MouseEvent const& e) {
    if (e.mods.isMiddleButtonDown()) {
        getParentComponent()->mouseDown(e);
        setMouseCursor(MouseCursor::DraggingHandCursor);
        return;
    }
    auto cnv = findParentComponentOfClass<EditorNodeCanvas>();
    cnv->setSingleSelected(this);
    mouseDownPos = e.getScreenPosition();
}

void EditorNode::mouseDrag(MouseEvent const &e) {
    //if (e.originalComponent != this)
    //    return;

    if (!e.mods.isLeftButtonDown()) {
        getParentComponent()->mouseDrag(e.getEventRelativeTo(getParentComponent()));
        return;
    }

    int offset = 0;

    if (e.originalComponent == this) {
        auto cnv = findParentComponentOfClass<EditorNodeCanvas>();
        //if (cnv->selectedComponents.getNumSelected() >= 2) {
            for (auto &node: cnv->selectedComponents) {
                if (node != this) {
                    node->mouseDownPos = mouseDownPos;
                    node->mouseDrag(e.getEventRelativeTo(getParentComponent()));

                }
            }
        //}
    }

    //dragDelta =  e.getScreenPosition() - mouseDownPos;
    //std::cout << "pos: " << pos.toString() << std::endl;
    //setTopLeftPosition(pos - dragDelta);
    pos += e.getScreenPosition() - mouseDownPos;
    GraphStateUpdater::getInstance().queueUpdate(this, [this]() {
        setTopLeftPosition(pos);
    });
    //setTopLeftPosition(pos);
    mouseDownPos = e.getScreenPosition();
    //dragDelta = mouseDownPos - e.getOffsetFromDragStart();
}

void EditorNode::renderNVG(NVGWrapper *nvgWrapper) {
    auto nvg = nvgWrapper->nvg;

#ifdef USE_JUCE_TEXT_RENDER
    auto bitmapImage = Image::BitmapData(textTex, 0, 0, 200, 60, Image::BitmapData::readOnly);
    if (nvgImage == -1) {
/*
        uint8 * px = data;
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 30; j++) {
                px[0] = bitmapImage.getPixelColour(i,j).getRed();
                px[1] = bitmapImage.getPixelColour(i,j).getGreen();
                px[2] = bitmapImage.getPixelColour(i,j).getBlue();
                px[3] = bitmapImage.getPixelColour(i,j).getAlpha();
                px += 4;
            }
        }
*/
        nvgImage = nvgCreateImageRGBA(nvg, 200, 60, 2, bitmapImage.data);
    } else if (textNeedsUpdate){
        nvgUpdateImage(nvg, nvgImage, bitmapImage.data);
        textNeedsUpdate = false;
    }

#endif
    auto cnv = findParentComponentOfClass<EditorNodeCanvas>();
    auto bounds = cnv->getLocalArea(this, getLocalBounds());
    auto braw = Rectangle<float>(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
    auto b = braw.reduced(border + .5f);

    auto cSelect = nvgRGBf(.3, .3, .3);
    auto cDefault = nvgRGBf(.2, .2, .2);
    auto cOutline = nvgRGBf(.6, .6, .6);

    auto colour = cDefault;
    auto selectedColour = nvgRGB(66, 162, 200);
    auto textColour = nvgRGBf(.8, .8, .8);

    auto isActivity = false;

    auto cTransparent = nvgRGBAf(0, 0, 0, 0);

    // glow
    if (isActivity) {
        nvgBeginPath(nvg);
        auto ds = b.expanded(4);
        //nvgRoundedRect(nvg, ds.getX(), ds.getY(), ds.getWidth(), ds.getHeight(), 8);
        if (glowDirty) {
            glow = nvgBoxGradient(nvg, ds.getX(), ds.getY(), ds.getWidth(), ds.getHeight(), 10, 20, cTransparent,
                                  cTransparent);
            glowDirty = false;
        }
        nvgFillPaint(nvg, glow);
        nvgFill(nvg);
    }

#ifdef USE_JUCE_TEXT_RENDER
    if (isSelected) {
        nvgStrokeColor(nvg, selectedColour);
        nvgStrokeWidth(nvg, 8.0f);
        nvgStroke(nvg);
    }

    auto imgPaint = nvgImagePattern(nvg, b.getX(), b.getY(), 100, 30, 0, nvgImage, 1.0f);
    nvgFillColor(nvg, colour);
    nvgFill(nvg);
    nvgFillPaint(nvg, imgPaint);
    nvgFill(nvg);
#else
    // draw selection corners
    if (isSelected) {
        nvgFillColor(nvg, selectedColour);
        // top left
        nvgBeginPath(nvg);
        nvgRoundedRect(nvg, b.getX() - 4, b.getY() - 4, 14, 14, 5);
        nvgFill(nvg);
        // bottom right
        nvgBeginPath(nvg);
        nvgRoundedRect(nvg, b.getX() + b.getWidth() - 10, b.getY() + b.getHeight() - 10, 14, 14, 5);
        nvgFill(nvg);
        // top right
        nvgBeginPath(nvg);
        nvgRoundedRect(nvg, b.getX() + b.getWidth() - 10, b.getY() - 4, 14, 14, 5);
        nvgFill(nvg);
        // bottom left
        nvgBeginPath(nvg);
        nvgRoundedRect(nvg, b.getX() - 4, b.getY() + b.getHeight() - 10, 14, 14, 5);
        nvgFill(nvg);

    }

    // draw node background with outline
    nvgBeginPath(nvg);
    nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 5);
    nvgFillColor(nvg, cDefault);
    nvgFill(nvg);
    nvgStrokeWidth(nvg, 1.f);
    nvgStrokeColor(nvg, isSelected ? selectedColour : cOutline);
    nvgStroke(nvg);

    // draw text
    nvgFillColor(nvg, textColour);
    nvgFontSize(nvg, 16.0f);
    nvgFontFace(nvg, "sans");
    nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
    nvgText(nvg, b.toFloat().getX() + 5, b.toFloat().getCentreY(),
            String("node~ " + String(thisNodeNumber)).toRawUTF8(), nullptr);

    // draw all iolets at once
    for (auto &inlet: inlets) {
        if (inlet->isActive) {
            nvgBeginPath(nvg);
            nvgFillColor(nvg, cOutline);
            nvgCircle(nvg, inlet->pos.x + braw.getX(), inlet->pos.y + braw.getY(), 8);
            nvgFill(nvg);

            nvgBeginPath(nvg);
            nvgFillColor(nvg, selectedColour);
            nvgCircle(nvg, inlet->pos.x + braw.getX(), inlet->pos.y +  braw.getY(), 7);
            nvgFill(nvg);
        } else {
            nvgBeginPath(nvg);
            nvgFillColor(nvg, cOutline);
            nvgArc(nvg, inlet->pos.x + braw.getX(), inlet->pos.y +  braw.getY(), 6, 0, NVG_PI, NVG_CW);
            nvgFill(nvg);

            nvgBeginPath(nvg);
            nvgFillColor(nvg, selectedColour);
            nvgArc(nvg, inlet->pos.x + braw.getX(), inlet->pos.y +  braw.getY(), 5, 0, NVG_PI, NVG_CW);
            nvgFill(nvg);
        }
    }

    // draw all outlets at once
    for (auto &outlet: outlets) {
        if (outlet->isActive) {
            nvgBeginPath(nvg);
            nvgFillColor(nvg, cOutline);
            nvgCircle(nvg, outlet->pos.x + braw.getX(), outlet->pos.y + braw.getY(), 8);
            nvgFill(nvg);

            nvgBeginPath(nvg);
            nvgFillColor(nvg, selectedColour);
            nvgCircle(nvg, outlet->pos.x + braw.getX(), outlet->pos.y + braw.getY(), 7);
            nvgFill(nvg);
        } else {
            nvgBeginPath(nvg);
            nvgFillColor(nvg, cOutline);
            nvgArc(nvg, outlet->pos.x + braw.getX(), outlet->pos.y + braw.getY(), 6, 0, NVG_PI, NVG_CCW);
            nvgFill(nvg);

            nvgBeginPath(nvg);
            nvgFillColor(nvg, selectedColour);
            nvgArc(nvg, outlet->pos.x + braw.getX(), outlet->pos.y + braw.getY(), 5, 0, NVG_PI, NVG_CCW);
            nvgFill(nvg);
        }
    }
#endif
}