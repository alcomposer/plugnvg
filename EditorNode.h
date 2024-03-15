//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include "NVGComponent.h"
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
            auto inlet = new EditorNodeIolet(this, i, EditorNodeIolet::Iolet::Inlet);
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
            auto outlet = new EditorNodeIolet(this, i, EditorNodeIolet::Iolet::Outlet);
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
#ifdef DEBUG_OBJECT_OUTLINE
        g.setColour(Colours::red);
        g.drawRect(getLocalBounds(), 1.0f);
#endif
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

    void renderNVG(NVGWrapper* nvgWrapper) override;

    void applyGammaCorrection(juce::Image &image, float gamma) {
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