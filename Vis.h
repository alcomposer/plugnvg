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

        //addAndMakeVisible(testBox);
        testBox.setSize(10,10);
        //startTimerHz(60);
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
        OpenGLHelpers::clear(Colours::transparentBlack);
        shader.reset();

        mousePos = getLocalPoint(nullptr, Desktop::getInstance().getMousePosition());
        testBox.setCentrePosition(mousePos);

        const auto fragmentShader = String("vec2 mousePos = vec2(") + String(static_cast<GLfloat>(mousePos.x)) + ", " + String(static_cast<GLfloat>(mousePos.y)) + ");\n" +
                                    String("vec2 iResolution = vec2(") + String(static_cast<GLfloat>(getWidth())) + ", " + String(static_cast<GLfloat>(getHeight())) + ");\n" +
                                    edges;

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
            //gl::glEnable(gl::GL_BLEND);
            //gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);
            shader->fillRect (g.getInternalContext(), getLocalBounds());
            //gl::glDisable(gl::GL_BLEND);
        }
    }
private:

    //OpenGLContext openGLContext;
    Point<int> mousePos;

    std::unique_ptr<OpenGLGraphicsContextCustomShader> shader;

    TestBox testBox;
    JUCE_MEDIUMP

    const String fragmentShader1 = R"(
        void main(){
            vec4 colour1 = vec4 (1.0, 1.0, 0.6, 1.0);
            vec4 colour2 = vec4 (0.0, 0.0, 0.0, 0.0);
            float alpha = distance (pixelPos, mousePos) / 20.0;
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

    const String edges = R"(
    // from https://iquilezles.org/articles/distfunctions
float roundedBoxSDF(vec2 CenterPosition, vec2 Size, float Radius) {
        return length(max(abs(CenterPosition)-Size+Radius,0.0))-Radius;
    }
    void main() {
        // The pixel space scale of the rectangle.
        vec2 size = iResolution.xy - vec2(38,38);

        // the pixel space location of the rectangle.
        vec2 location = (size.xy / vec2(2,2));
        location -= location - vec2(19,19);

        // How soft the edges should be (in pixels). Higher values could be used to simulate a drop shadow.
        float edgeSoftness  = 1.0f;

        // The radius of the corners (in pixels).
        float radius = 15.0f;

        // Calculate distance to edge.
        float distance 		= roundedBoxSDF(pixelPos.xy - location - (size/2.0f), size / 2.0f, radius);

        // Smooth the result (free antialiasing).
        float smoothedAlpha =  1.0f-smoothstep(0.0f, edgeSoftness * 2.0f,distance);

        // Return the resultant shape.
        vec4 quadColor		= mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(0.3f, 0.3f, 0.3f, smoothedAlpha), smoothedAlpha);

        // Apply a drop shadow effect.
        float shadowSoftness = 20.0f;
        vec2 shadowOffset 	 = vec2(0.0f, 0.0f);
        float shadowDistance = roundedBoxSDF(pixelPos.xy - location + shadowOffset - (size/2.0f), (size / 2.0f) - (shadowOffset * 2.0f), radius);
        float shadowAlpha 	 = 1.0f-smoothstep(-shadowSoftness, shadowSoftness, shadowDistance);
        vec4 shadowColor 	 = vec4(0.0f, 0.0f, 0.0f, 0.2f);
        gl_FragColor 	     = mix(quadColor, shadowColor, shadowAlpha - smoothedAlpha);
    }


    )";

    const String shaderToy = R"(
    float roundedBoxSDF(vec2 CenterPosition, vec2 Size, vec4 Radius)
    {
        Radius.xy = (CenterPosition.x > 0.0) ? Radius.xy : Radius.zw;
        Radius.x  = (CenterPosition.y > 0.0) ? Radius.x  : Radius.y;

        vec2 q = abs(CenterPosition)-Size+Radius.x;
        return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - Radius.x;
    }

    void mainImage( out vec4 fragColor, in vec2 fragCoord )
    {
        // =========================================================================
        // Inputs (uniforms)

        vec2  u_rectSize   = vec2(250.0, 250.0);     // The pixel-space scale of the rectangle.
        vec2  u_rectCenter = (iResolution.xy / 2.0); // The pixel-space rectangle center location

        float u_edgeSoftness   = 2.0; // How soft the edges should be (in pixels). Higher values could be used to simulate a drop shadow.
        vec4  u_cornerRadiuses = vec4(10.0, 20.0, 40.0, 60.0); // The radiuses of the corners(in pixels): [topRight, bottomRight, topLeft, bottomLeft]

        // Border
        float u_borderThickness = 5.0; // The border size (in pixels)
        float u_borderSoftness  = 2.0; // How soft the (internal) border should be (in pixels)

        // Shadow
        float u_shadowSoftness = 30.0;            // The (half) shadow radius (in pixels)
        vec2  u_shadowOffset   = vec2(0.0, 10.0); // The pixel-space shadow offset from rectangle center

        // Colors
        vec4  u_colorBg     = vec4(0.93, 0.93, 0.93, 1.0); // The color of background
        vec4  u_colorRect   = vec4(1.0,  0.30, 0.45, 1.0); // The color of rectangle
        vec4  u_colorBorder = vec4(0.7,  0.25, 0.55, 1.0); // The color of (internal) border
        vec4  u_colorShadow = vec4(0.4,  0.4,  0.4,  1.0); // The color of shadow

        // =========================================================================

        vec2 halfSize = (u_rectSize / 2.0); // Rectangle extents (half of the size)

        vec4 radius = vec4((sin(iTime) + 1.0)) * u_cornerRadiuses; // Animated corners radiuses

        // -------------------------------------------------------------------------

        // Calculate distance to edge.
        float distance = roundedBoxSDF(fragCoord.xy - u_rectCenter, halfSize, radius);

        // Smooth the result (free antialiasing).
        float smoothedAlpha = 1.0-smoothstep(0.0, u_edgeSoftness, distance);

        // -------------------------------------------------------------------------
        // Border.

        float borderAlpha   = 1.0-smoothstep(u_borderThickness - u_borderSoftness, u_borderThickness, abs(distance));

        // -------------------------------------------------------------------------
        // Apply a drop shadow effect.

        float shadowDistance  = roundedBoxSDF(fragCoord.xy - u_rectCenter + u_shadowOffset, halfSize, radius);
        float shadowAlpha 	  = 1.0-smoothstep(-u_shadowSoftness, u_shadowSoftness, shadowDistance);


        // -------------------------------------------------------------------------
        // Debug output

            // vec4 debug_sdf = vec4(distance, 0.0, 0.0, 1.0);

            // Notice, that instead simple 'alpha' here is used 'min(u_colorRect.a, alpha)' to enable transparency
            // vec4 debug_rect_color   = mix(u_colorBg, u_colorRect, min(u_colorRect.a, smoothedAlpha));

            // Notice, that instead simple 'alpha' here is used 'min(u_colorBorder.a, alpha)' to enable transparency
            // vec4 debug_border_color = mix(u_colorBg, u_colorBorder, min(u_colorBorder.a, min(borderAlpha, smoothedAlpha)) );

        // -------------------------------------------------------------------------
        // Apply colors layer-by-layer: background <- shadow <- rect <- border.

        // Blend background with shadow
        vec4 res_shadow_color = mix(u_colorBg, vec4(u_colorShadow.rgb, shadowAlpha), shadowAlpha);

        // Blend (background+shadow) with rect
        //   Note:
        //     - Used 'min(u_colorRect.a, smoothedAlpha)' instead of 'smoothedAlpha'
        //       to enable rectangle color transparency
        vec4 res_shadow_with_rect_color =
            mix(
                res_shadow_color,
                u_colorRect,
                min(u_colorRect.a, smoothedAlpha)
            );

        // Blend (background+shadow+rect) with border
        //   Note:
        //     - Used 'min(borderAlpha, smoothedAlpha)' instead of 'borderAlpha'
        //       to make border 'internal'
        //     - Used 'min(u_colorBorder.a, alpha)' instead of 'alpha' to enable
        //       border color transparency
        vec4 res_shadow_with_rect_with_border =
            mix(
                res_shadow_with_rect_color,
                u_colorBorder,
                min(u_colorBorder.a, min(borderAlpha, smoothedAlpha))
            );

        // -------------------------------------------------------------------------

        fragColor = res_shadow_with_rect_with_border;
    }
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
