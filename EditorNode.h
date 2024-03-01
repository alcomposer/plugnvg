//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include "EditorNodeIolet.h"
#include "libraries/melatonin_perfetto/melatonin_perfetto/melatonin_perfetto.h"

//#define USE_JUCE_TEXT_RENDER

class EditorNode : public NVGComponent, public Timer
{
public:
    EditorNode(Point<int> pos) : pos(pos)
    {
        //setInterceptsMouseClicks(true, true);
        static int nodeNumber = 0;
        thisNodeNumber = nodeNumber++;
        auto name = String("node: " + String(thisNodeNumber));
        setName(name);
        setSize(100 + (border * 2), 30 + (border * 2));
        setTopLeftPosition(pos);

        // setup iolets
        auto inletNum = 2;
        auto inletPos = 20;
        auto inletPadding = (getBounds().getWidth() - (2 * border) + 20) / (inletNum);
        for (int i = 0; i < inletNum; i++) {
            auto inlet = new EditorNodeIolet(EditorNodeIolet::Iolet::Inlet);
            inlets.add(inlet);
            addAndMakeVisible(inlet);
            inlet->setCentrePosition(inletPos + border, border);
            inlet->pos.x = inletPos + border;
            inlet->pos.y = border;
            inletPos += inletPadding;
        }

        // setup outlets
        auto outletPos = 20;
        auto outletPadding = (getBounds().getWidth() - (2 * 20) + (2 * border)) / 3;
        for (int i = 0; i < 3; i++) {
            auto outlet = new EditorNodeIolet(EditorNodeIolet::Iolet::Outlet);
            outlets.add(outlet);
            addAndMakeVisible(outlet);
            outlet->setCentrePosition(outletPos + border, getHeight() - border);
            outlet->pos.x = outletPos + border;
            outlet->pos.y = getHeight() - border;
            outletPos += outletPadding;
        }

        updateText();
        //startTimerHz(1);
    }

    ~EditorNode(){};

    void timerCallback() override
    {
        updateText();
    }

    void paintOverChildren(Graphics& g) override
    {
        return;
        g.setColour(Colours::red);
        g.drawRect(getLocalBounds(), 1.0f);
    }

    void updateText(String text = String())
    {
#ifdef USE_JUCE_TEXT_RENDER
        static int nodeNumber = 0;
        auto name = String("node: " + String(nodeNumber++));

        textTex = Image(Image::PixelFormat::ARGB, 200, 60, true);
        auto g = Graphics(textTex);
        g.setFont(juce::Font(36.0f));
        g.setColour(Colour(255 * .9f, 255 * .9f, 255 * .9f));
        g.drawRoundedRectangle(1.f, 1.f, 198.f, 58.f, 10.0f, 8.0f);
        g.drawText(name, Rectangle<int>(10, 0, 195, 60), Justification::centredLeft);

        textNeedsUpdate = true;
#endif
    }

    WidgetType getType() override
    {
        return WidgetType::Node;
    }

    void mouseEnter(MouseEvent const& e) override
    {
        isHover = true;
    }

    void mouseExit(MouseEvent const& e) override
    {
        isHover = false;
    }

    void mouseDown(MouseEvent const& e) override;

    void mouseUp(MouseEvent const& e) override
    {
        setCentrePosition(pos);
        setMouseCursor(MouseCursor::NormalCursor);
    }

    bool hitTest(int x, int y) override
    {
        if (getLocalBounds().reduced(border).contains(x, y))
            return true;

        for (auto& inlet : inlets) {
            if (inlet->getBoundsInParent().contains(x, y))
                return true;
        }

        for (auto& outlet : outlets) {
            if (outlet->getBoundsInParent().contains(x, y))
                return true;
        }

        return false;
    }

    void mouseDrag(MouseEvent const& e) override;

    Rectangle<int> getSelectableBounds()
    {
        return getBounds().reduced(border);
    }

    void renderNVG(NVGWrapper* nvgWrapper) override
    {
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

        //auto b = getBounds().translated(parentLeft.getX(), parentLeft.getY()).reduced(border);
        auto b = Rectangle<float>(pos.x, pos.y, getWidth(), getHeight()).reduced(border + .5f);
        //auto b = Rectangle<int>(pos.x, pos.y, getWidth(), getHeight()).reduced(border);
        setTopLeftPosition(pos);
        //nvgBeginPath(nvg);
        auto cSelect = nvgRGBf(.3, .3, .3);
        auto cDefault = nvgRGBf(.2, .2, .2);

        // we need to check if the var pointer is valid before we dereference it!
        //if (auto selected = getProperties().getVarPointer("is_selected"))
        //    isSelected = static_cast<bool>(*selected);

        auto colour = cDefault;
        auto selectedColour = nvgRGB(66, 162, 200);

        auto isActivity = false;

        auto cTransparent = nvgRGBAf(0, 0, 0, 0);

        // glow
        if (isActivity) {
            nvgBeginPath(nvg);
            auto ds = b.expanded(4);
            //nvgRoundedRect(nvg, ds.getX(), ds.getY(), ds.getWidth(), ds.getHeight(), 8);
            if (glowDirty) {
                glow = nvgBoxGradient(nvg, ds.getX(), ds.getY(), ds.getWidth(), ds.getHeight(), 10, 20, cTransparent, cTransparent);
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

        nvgBeginPath(nvg);
        nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 5);
        nvgFillColor(nvg, cDefault);
        nvgFill(nvg);
        nvgStrokeWidth(nvg, 1.f);
        nvgStrokeColor(nvg, isSelected ? selectedColour : nvgRGBf(.8, .8, .8));
        nvgStroke(nvg);

        nvgFillColor(nvg, nvgRGBf(.8, .8, .8));
        nvgFontSize(nvg, 16.0f);
        nvgFontFace(nvg, "sans");
        nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
        nvgText(nvg, b.toFloat().getX() + 5, b.toFloat().getCentreY(), String("node~ " + String(thisNodeNumber)).toRawUTF8(), nullptr);

        // draw all iolets at once
        for (auto& inlet : inlets) {
            if (inlet->isActive) {
                nvgBeginPath(nvg);
                nvgFillColor(nvg, selectedColour);
                nvgCircle(nvg, inlet->pos.x + pos.x, inlet->pos.y + pos.y, 7);
                nvgFill(nvg);
            } else {
                nvgBeginPath(nvg);
                nvgFillColor(nvg, nvgRGBf(.8, .8, .8));
                nvgArc(nvg, inlet->pos.x + pos.x, inlet->pos.y + pos.y, 6, 0, NVG_PI, NVG_CW);
                nvgFill(nvg);

                nvgBeginPath(nvg);
                nvgFillColor(nvg, selectedColour);
                nvgArc(nvg, inlet->pos.x + pos.x, inlet->pos.y + pos.y, 5, 0, NVG_PI, NVG_CW);
                nvgFill(nvg);
            }
        }

        // draw all outlets at once
        for (auto& outlet : outlets) {
            if (outlet->isActive) {
                nvgBeginPath(nvg);
                nvgFillColor(nvg, selectedColour);
                nvgCircle(nvg, outlet->pos.x + pos.x, outlet->pos.y + pos.y, 7);
                nvgFill(nvg);
            } else {
                nvgBeginPath(nvg);
                nvgFillColor(nvg, nvgRGBf(.8, .8, .8));
                nvgArc(nvg, outlet->pos.x + pos.x, outlet->pos.y + pos.y, 6, 0, NVG_PI, NVG_CCW);
                nvgFill(nvg);

                nvgBeginPath(nvg);
                nvgFillColor(nvg, selectedColour);
                nvgArc(nvg, outlet->pos.x + pos.x, outlet->pos.y + pos.y, 5, 0, NVG_PI, NVG_CCW);
                nvgFill(nvg);
            }
        }
#endif
    }

    void applyGammaCorrection(juce::Image& image, float gamma) {
        juce::Image::BitmapData bitmapData(image, juce::Image::BitmapData::writeOnly);

    // Iterate through each pixel in the image
    for (int y = 0; y < bitmapData.height; ++y) {
        for (int x = 0; x < bitmapData.width; ++x) {
            // Get the color of the pixel
            juce::Colour pixelColour(bitmapData.getPixelColour(x, y));

            // Apply gamma correction to each color channel
            float r = std::pow(pixelColour.getFloatRed(), gamma);
            float g = std::pow(pixelColour.getFloatGreen(), gamma);
            float b = std::pow(pixelColour.getFloatBlue(), gamma);
            float a = pixelColour.getFloatAlpha();

            // Set the corrected color to the pixel
            bitmapData.setPixelColour(x, y, Colour(PixelARGB(a * 255, r * 255, g * 255, b * 255)));
        }
    }
}

// Function to convert gamma corrected image to linear space
void convertToLinear(juce::Image& image, float gamma) {
    // Apply inverse gamma correction to convert back to linear space
    applyGammaCorrection(image, 1.0f / gamma);
}

    bool isSelected = false;

    Point<int> mouseDownPos;

    OwnedArray<EditorNodeIolet> inlets;
    OwnedArray<EditorNodeIolet> outlets;

private:
    Image textTex;
    int thisNodeNumber;
    bool textNeedsUpdate = false;
    unsigned char data[100*30*4];
    int nvgImage = -1;
    Point<int> pos;
    bool isHover = false;
    float border = 8;

    NVGpaint glow;
    bool glowDirty = true;

    Point<int> dragDelta;
};