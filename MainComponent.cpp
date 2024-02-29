#include "MainComponent.h"
#include <stack>
#include <mutex>
#include <functional>
#include <vector>

#include "libraries/nanovg-dev/src/nanovg.h"
#define NANOVG_GLES3_IMPLEMENTATION
#include "libraries/nanovg-dev/src/nanovg_gl.h"

//#include "NVGComponent.h"
//#include "Editor.h"
//==============================================================================
MainComponent::MainComponent()
{
#if PERFETTO
    std::cout << "=============== RUNNING PERFETTO ==============" << std::endl;
    MelatoninPerfetto::get().beginSession(300000);
#endif
    //auto* mainWindow = findParentComponentOfClass<juce::DocumentWindow>();
    //resizableBorderComponent = std::make_unique<juce::ResizableBorderComponent>(mainWindow, nullptr);
    //resizableBorderComponent->setAlwaysOnTop(true);
    ////resized();
    //mainWindow->Component::addAndMakeVisible(resizableBorderComponent.get());
    //setOpaque(false);

    //lock = std::make_shared<std::mutex>();
    if (auto *peer = getPeer())
        peer->setCurrentRenderingEngine(0);


    glContext.setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
    glContext.setComponentPaintingEnabled(true);

    glContext.setMultisamplingEnabled(false);
    /*
    auto form = OpenGLPixelFormat(8,8,16,8);
    form.multisamplingLevel = 2;
    glContext.setPixelFormat(form);
    */
    glContext.setContinuousRepainting(false);
    //glContext.setSwapInterval(1);

    //glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE );
    glContext.setRenderer(this);
    glContext.attachTo(*this);

    editor = std::make_unique<Editor>();
    addAndMakeVisible(editor.get());

    setBounds(juce::Rectangle<int>(0,0,1200, 600));
    startTimerHz(60);
}

MainComponent::~MainComponent()
{
#if PERFETTO
    std::cout << "================ ENDING PERFETTO =================" << std::endl;
    MelatoninPerfetto::get().endSession();
#endif
}

void MainComponent::timerCallback()
{
    glContext.triggerRepaint();
}

void MainComponent::newOpenGLContextCreated()
{
    nvg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (!nvg)
        std::cout << "could not init nvg" << std::endl;

    nvgWrapper.nvg = nvg;
    nvgWrapper.mousePosScreen = Desktop::getInstance().getMousePosition();

    nvgWrapper.interFont = nvgCreateFont(nvg, "sans", "../../../Data/InterSemiBold.ttf");
    if (nvgWrapper.interFont == -1)
        std::cout << "could not init font" << std::endl;
}

void MainComponent::openGLContextClosing()
{
    glContext.detach();
}

void MainComponent::renderOpenGL()
{
    //TRACE_COMPONENT();
    ScopedLock lock(renderLock);
    glViewport(0, 0, getWidth(), getHeight());
    OpenGLHelpers::clear(Colours::black);

    nvgBeginFrame(nvg, getWidth(), getHeight(), 1.0f);
    processRender(editor.get());
    nvgEndFrame(nvg);
}

void MainComponent::processRender(Component* c)
{
    TRACE_COMPONENT();
    if (c == nullptr || !c->isVisible())
        return;

    NVGComponent* nvgComp = nullptr;

    if (nvgComp = dynamic_cast<NVGComponent*>(c)) {
        //if (nvgComp->getType() != NVGComponent::WidgetType::Iolet)
            nvgComp->render(&nvgWrapper);
        // render everything for now
        /*
        auto type = nvgComp->getType();
        if ((type == NVGComponent::WidgetType::Node) || (type == NVGComponent::WidgetType::Iolet)) {
            if (nvgComp->getScreenBounds().intersects(editor->getScreenBounds())) {
                nvgComp->render(&nvgWrapper);
            }
        } else {
            nvgComp->render(&nvgWrapper);
        }
         */
    }

    //if (c->getNumChildComponents() > 0) {
        for (auto& child: c->getChildren()) {
            processRender(child);
        }
    //}

    if (nvgComp)
        nvgComp->resetNVG(nvg);
}

void MainComponent::processRenderStack(Component* root)
{
    //TRACE_COMPONENT();
    if (root == nullptr)
        return;

    std::stack<Component*> componentsStack;
    componentsStack.push(root);

    while (!componentsStack.empty()) {
        auto* currentComponent = componentsStack.top();
        componentsStack.pop();

        if (!currentComponent->isVisible())
            continue;

        if (auto nvcComp = dynamic_cast<NVGComponent *>(currentComponent)) {
            nvcComp->render(&nvgWrapper);
        }

        for (auto& child : currentComponent->getChildren())
            componentsStack.push(child);
    }
}

void MainComponent::processRenderVector(Component* root)
{
    //TRACE_COMPONENT();
    if (root == nullptr)
        return;

    std::vector<Component*> stack;
    stack.push_back(root);

    std::vector<NVGComponent*> nvgComponents; // Collect NVGComponent pointers

    while (!stack.empty()) {
        Component* current = stack.back();
        stack.pop_back();

        if (!current->isVisible())
            continue;

        if (auto nvcComp = reinterpret_cast<NVGComponent*>(current)) {
            nvgComponents.push_back(nvcComp); // Store NVGComponents
        }

        if (current->getNumChildComponents() > 0) {
            for (Component* child : current->getChildren()) {
                stack.push_back(child);
            }
        }
    }

    // Now, render the collected NVGComponents
    for (auto it = nvgComponents.rbegin(); it != nvgComponents.rend(); ++it) {
       auto& node = *it;
       node->render(&nvgWrapper);
    }
}

//==============================================================================


void MainComponent::resized()
{
    editor->setBounds(getBounds());
}
