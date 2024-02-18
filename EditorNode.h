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
        setInterceptsMouseClicks(true, false);
        static int nodeNumber = 0;
        auto name = String("node: " + String(nodeNumber++));
        setName(name);
        setSize(100, 30);
        setCentrePosition(pos);
        auto ioletPos = 5;
        for (int i = 0; i < 3; i++) {
            auto iolet = new EditorNodeIolet();
            iolets.add(iolet);
            addAndMakeVisible(iolet);
            iolet->setCentrePosition(ioletPos, 0);
            iolet->pos.x = ioletPos;
            ioletPos += 15;
        }

        textTex = Image(Image::PixelFormat::ARGB, 100, 30, true);
        auto g = Graphics(textTex);
        g.setFont(juce::Font(18.0f));
        g.setColour(Colours::black);
        g.drawText(name, Rectangle<int>(5,0,95,30), Justification::centredLeft);
    }

    void mouseEnter(MouseEvent const& e) override
    {
        isHover = true;
    }

    void mouseExit(MouseEvent const& e) override
    {
        isHover = false;
    }

    void mouseDrag(MouseEvent const& e) override
    {
        //if (lock.tryEnter())
        if (e.originalComponent != this)
            return;
        pos = e.getScreenPosition() - getParentComponent()->getScreenPosition();
        //std::cout << "pos: " << pos.toString() << std::endl;
        setCentrePosition(pos);
    }

    void renderNVG(NVGcontext* nvg) override
    {
        auto bitmapImage = Image::BitmapData(textTex, 0, 0, 100, 30, Image::BitmapData::readOnly);
        if (nvgImage == -1) {

            /*
            unsigned char * px = data;
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
        } else {
            /*
            unsigned char c = (unsigned char)((1+sinf(t))*0.5f*255.0f);
            unsigned char* px = data;
            for (i = 0; i < IMG_SIZE; i++) {
                for (j = 0; j < IMG_SIZE; j++) {
                    unsigned char g = (((i>>1) ^ (j>>1)) & 1) * 255;
                    px[0] = c;
                    px[1] = c;
                    px[2] = c;
                    px[3] = g;
                    px += 4;
                }
            }
             */
            nvgUpdateImage(nvg, nvgImage, bitmapImage.data);
        }



        auto parentLeft = getParentComponent()->getBounds().getTopLeft();
        auto b = getBounds().translated(parentLeft.getX(), parentLeft.getY());
        nvgBeginPath(nvg);
        auto cSelect = nvgRGBf(.3, .3, .3);
        auto cDefault = nvgRGBf(.2, .2, .2);
        auto colour = isHover ? cSelect : cDefault;
        nvgRoundedRect(nvg, b.getX(), b.getY(), b.getWidth(), b.getHeight(), 5);
        auto imgPaint = nvgImagePattern(nvg, b.getX(), b.getY(), 100, 30, 0, nvgImage, 1.0f);
        nvgFillColor(nvg, colour);
        nvgFill(nvg);
        nvgFillPaint(nvg, imgPaint);
        nvgFill(nvg);
        nvgStrokeColor(nvg, nvgRGBf(.9, .9, .9));
        nvgStrokeWidth(nvg, 1.f);
        nvgStroke(nvg);
        nvgClosePath(nvg);
    }
private:
    Image textTex;
    unsigned char data[100*30*4];
    int nvgImage = -1;
    Point<int> pos;
    bool isHover = false;
    OwnedArray<EditorNodeIolet> iolets;
};