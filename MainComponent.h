#pragma once



// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <JuceHeader.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "libraries/melatonin_perfetto/melatonin_perfetto/melatonin_perfetto.h"

#include <juce_opengl/juce_opengl.h>
using namespace juce::gl;

#include "Editor.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public OpenGLRenderer, public Component, public Timer
{
public:
    MainComponent();

    ~MainComponent();

    void resized() override;

    void timerCallback() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void processRender(Component* node);
    void processRenderStack(Component* node);
    void processRenderVector(Component* node);

    CriticalSection renderLock;

private:
    OpenGLContext glContext;
    NVGcontext* nvg;
    NVGWrapper nvgWrapper;

    std::unique_ptr<Editor> editor;

#if PERFETTO
    std::unique_ptr<perfetto::TracingSession> tracingSession;
#endif

    std::unique_ptr<juce::ComponentBoundsConstrainer> constrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
