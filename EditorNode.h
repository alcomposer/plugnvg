//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include "EditorNodeIolet.h"
#include "libraries/melatonin_perfetto/melatonin_perfetto/melatonin_perfetto.h"

class EditorNode : public NVGComponent
{
public:
    EditorNode(Point<int> pos) : pos(pos)
    {
        //setInterceptsMouseClicks(true, true);
        static int nodeNumber = 0;
        auto name = String("node: " + String(nodeNumber++));
        setName(name);
        setSize(100 + (border * 2), 30 + (border * 2));
        setTopLeftPosition(pos);

        // setup iolets
        auto inletPos = 10;
        for (int i = 0; i < 3; i++) {
            auto inlet = new EditorNodeIolet(EditorNodeIolet::Iolet::Inlet);
            inlets.add(inlet);
            addAndMakeVisible(inlet);
            inlet->setCentrePosition(inletPos + border, border);
            inlet->pos.x = inletPos + border;
            inlet->pos.y = border;
            inletPos += 15;
        }

        // setup outlets
        auto outletPos = 10;
        for (int i = 0; i < 3; i++) {
            auto outlet = new EditorNodeIolet(EditorNodeIolet::Iolet::Outlet);
            outlets.add(outlet);
            addAndMakeVisible(outlet);
            outlet->setCentrePosition(outletPos + border, getHeight() - border);
            outlet->pos.x = outletPos + border;
            outlet->pos.y = getHeight() - border;
            outletPos += 15;
        }

        textTex = Image(Image::PixelFormat::ARGB, 100, 30, true);
        auto g = Graphics(textTex);
        g.setFont(juce::Font(18.0f));
        g.setColour(Colour(255 * .9f, 255 * .9f, 255 * .9f));
        g.drawRoundedRectangle(1.f, 1.f, 98.f, 28.f, 5.0f, 2.0f);
        g.drawText(name, Rectangle<int>(5,0,95,30), Justification::centredLeft);
        //convertToLinear(textTex, 2.2f);
    }

    ~EditorNode(){};

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

    void renderNVG(NVGcontext* nvg) override
    {
        auto bitmapImage = Image::BitmapData(textTex, 0, 0, 100, 30, Image::BitmapData::readOnly);
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
            nvgImage = nvgCreateImageRGBA(nvg, 100, 30, 0, bitmapImage.data);
            std::cout << "image hash: " << nvgImage << std::endl;
        } else if (textNeedsUpdate){
            nvgUpdateImage(nvg, nvgImage, bitmapImage.data);
        }

        auto parentLeft = getParentComponent()->getBounds().getTopLeft();
        //auto b = getBounds().translated(parentLeft.getX(), parentLeft.getY()).reduced(border);
        auto b = Rectangle<int>(pos.x + parentLeft.x, pos.y + parentLeft.y, getWidth(), getHeight()).reduced(border);
        //auto b = Rectangle<int>(pos.x, pos.y, getWidth(), getHeight()).reduced(border);
        setTopLeftPosition(pos);
        //nvgTranslate(nvg, parentLeft.x, parentLeft.y);
        nvgBeginPath(nvg);
        auto cSelect = nvgRGBf(.3, .3, .3);
        auto cDefault = nvgRGBf(.2, .2, .2);

        // we need to check if the var pointer is valid before we dereference it!
        //if (auto selected = getProperties().getVarPointer("is_selected"))
        //    isSelected = static_cast<bool>(*selected);

        auto colour = cDefault;
        auto selectedColour = nvgRGBf(.3f, .3f, 1.f);


        nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 5);
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
        //nvgStrokeColor(nvg, nvgRGBf(.9, .9, .9));
        //nvgStrokeWidth(nvg, 1.f);
        //nvgStroke(nvg);
        //nvgClosePath(nvg);
        //nvgTranslate(nvg, -parentLeft.x, parentLeft.y);
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

private:
    Image textTex;
    bool textNeedsUpdate = false;
    unsigned char data[100*30*4];
    int nvgImage = -1;
    Point<int> pos;
    bool isHover = false;
    int border = 8;

    Point<int> dragDelta;
    OwnedArray<EditorNodeIolet> inlets;
    OwnedArray<EditorNodeIolet> outlets;
};