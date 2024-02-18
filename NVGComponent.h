//
// Created by alexmitchell on 16/02/24.
//

#pragma once

#include <JuceHeader.h>
#include <nanovg.h>

class NVGComponent : public Component
{
public:
    NVGComponent(NVGComponent* parent) : parent(parent) {}

    virtual void renderNVG(NVGcontext* nvg) = 0;

    void render(NVGcontext* nvg)
    {
        //lock.enter();
        TRACE_COMPONENT();
        renderNVG(nvg);
        //lock.exit();
    }

    void addNVGComponent(NVGComponent* c)
    {
        children.add(c);
    }

    Array<NVGComponent*> children;
    NVGComponent* parent;
private:

};