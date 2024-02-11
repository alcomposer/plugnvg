//
// Created by alexmitchell on 10/02/24.
//

#pragma once

#include <JuceHeader.h>

/*
 // Copyright (c) 2024 Timothy Schoen and Alex Mitchell
 // For information on usage and redistribution, and for a DISCLAIMER OF ALL
 // WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#pragma once

#include <JuceHeader.h>
#include <juce_opengl/juce_opengl.h>
#include "TestBox.h"

class OpenGLCon : public Component, public Timer
{
public:
    OpenGLCon(Component* parent)
    {
        setOpaque(false);

        addMouseListener(this, false);

        //openGLContext.setOpenGLVersionRequired(OpenGLContext::openGL4_3);
        //openGLContext.setMultisamplingEnabled(true);

        //auto pixelFormat = OpenGLPixelFormat(8, 8, 16, 8);
        //pixelFormat.multisamplingLevel = 2;
        //openGLContext.setPixelFormat(pixelFormat);

        //openGLContext.setComponentPaintingEnabled(true);
        //openGLContext.executeOnGLThread([](OpenGLContext& context){
        //    // We get unpredictable behaviour if the context is active on multiple threads
        //    OpenGLContext::deactivateCurrentContext();
        //}, true);
        //openGLContext.makeActive();
        //openGLContext.setContinuousRepainting(true);
        //openGLContext.attachTo(*parent);

        addAndMakeVisible(testBox);
        testBox.setSize(10,10);
        startTimerHz(60);
    }

    ~OpenGLCon()
    {
        //openGLContext.detach();
        shader.reset();
    }

    void mouseMove(const MouseEvent& e) override
    {
        //std::cout << "mousePos = " << e.getPosition().toString() << std::endl;
        //mousePos = e.getPosition();
        //repaint();
    }

    void timerCallback() override
    {
        repaint();
    }

    void resized() override
    {
        //auto b = Rectangle<int>(0,0,10,10).withCentre(getLocalBounds().getCentre());
        //testBox.setBounds(b);
    }


    void paint(Graphics& g) override
    {
        //auto image = Image(Image::RGB, 500, 500, true);
        //fb = OpenGLImageType::getFrameBufferFrom(image);
        shader.reset();

        mousePos = getLocalPoint(nullptr, Desktop::getInstance().getMousePosition());
        testBox.setCentrePosition(mousePos);

        const auto fragmentShader = String("vec2 mousePos = vec2(") + String(static_cast<GLfloat>(mousePos.x)) + ", " + String(static_cast<GLfloat>(mousePos.y)) + ");\n" + fragmentShader1;

        if (fragmentShader.isNotEmpty()) {
            //std::cout << "resetting the shader =====================" << std::endl;
            shader.reset(new OpenGLGraphicsContextCustomShader(fragmentShader));
            //shader->getProgram(g.getInternalContext())->setUniform("mousePos", static_cast<GLfloat>(mousePos.x), static_cast<GLfloat>(mousePos.y));
            auto result = shader->checkCompilation(g.getInternalContext());

            if (result.failed()) {
                std::cout << "failed: " << result.getErrorMessage() << std::endl;
                std::cout << "frag code is: " << fragmentShader << std::endl;
                shader.reset();
            }
        }
        if (shader.get() != nullptr)
        {
            //std::cout << "drawing" << std::endl;
            //shader->getProgram(g.getInternalContext())->setUniform("mousePos", static_cast<GLfloat>(mousePos.x), static_cast<GLfloat>(mousePos.y));
            shader->fillRect (g.getInternalContext(), getLocalBounds());
        }
    }
private:

    //OpenGLContext openGLContext;
    Point<int> mousePos;

    std::unique_ptr<OpenGLGraphicsContextCustomShader> shader;

    TestBox testBox;

    const String fragmentShader1 = R"(
        void main(){
            vec4 colour1 = vec4 (1.0, 0.4, 0.6, 1.0);
            vec4 colour2 = vec4 (0.3, 0.4, 0.4, 1.0);
            float alpha = distance (pixelPos, mousePos) / 200.0;
            gl_FragColor = pixelAlpha * mix (colour1, colour2, alpha);
        }
    )";

    const String fragmentShaderDots = R"(
        void main()
        {
        vec4 outColor = vec4 (1.0, 1.0, 1.0, 1.0);
        if ((mod(pixelPos.x - 1, 2) == 0) && (mod(pixelPos.y - 2, 2) == 0 )){
            outColor = vec4(1.0, 1.0, 1.0, 1.0);
        else
        outColor = vec4(0.0, 0.0, 0.0, 0.0);
        gl_FragColor = outColor;
        )";
};

class CanvasViewportBackground : public Component
{
public:
    CanvasViewportBackground(){
        glCon = std::make_unique<OpenGLCon>(this);
        addAndMakeVisible(glCon.get());
    }
    void resized() override
    {
        glCon->setBounds(getLocalBounds());
    }

private:
    std::unique_ptr<OpenGLCon> glCon;

};
