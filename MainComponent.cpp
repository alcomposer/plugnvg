#include "MainComponent.h"
#include <stack>
#include <mutex>
#include <functional>
#include <vector>

#include "libraries/nanovg-dev/src/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
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

    glContext.setComponentPaintingEnabled(false);
    glContext.setOpenGLVersionRequired(OpenGLContext::openGL3_2);
    //glContext.setMultisamplingEnabled(true);
    auto form = OpenGLPixelFormat(8,8,16,8);
    //form.multisamplingLevel = 2;
    glContext.setPixelFormat(form);
    glContext.setSwapInterval(1);
    glContext.setContinuousRepainting(true);
    //glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE );
    glContext.setRenderer(this);
    glContext.attachTo(*this);

    editor = std::make_unique<Editor>();
    addAndMakeVisible(editor.get());

    setSize (600, 600 * (9 / 16.0f));
    //startTimerHz(60);
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
    nvg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    if (!nvg)
        std::cout << "could not init nvg" << std::endl;
}

void MainComponent::openGLContextClosing()
{
    glContext.detach();
}

void MainComponent::renderOpenGL()
{
    ScopedLock lock(renderLock);
    TRACE_COMPONENT();
    glViewport(0, 0, getWidth(), getHeight());
    OpenGLHelpers::clear(Colours::red);

    nvgBeginFrame(nvg, getWidth(), getHeight(), 1.0f);
    processRender(editor.get());
    nvgEndFrame(nvg);
}

void MainComponent::processRender(Component* c)
{
    TRACE_COMPONENT();
    if (c == nullptr || !c->isVisible())
        return;

    if (auto* nvgComp = dynamic_cast<NVGComponent*>(c)) {
        auto type = nvgComp->getType();
        if ((type == NVGComponent::WidgetType::Node) || (type == NVGComponent::WidgetType::Iolet)) {
            if (nvgComp->getScreenBounds().intersects(editor->getScreenBounds())) {
                nvgComp->render(nvg);
                //std::cout << "rendering; " << nvgComp->getName() << std::endl;
            }
        } else {
            nvgComp->render(nvg);
            //std::cout << "rendering; " << nvgComp->getName() << std::endl;
        }
    }

    if (c->getNumChildComponents() > 0) {
        for (auto& child: c->getChildren())
            processRender(child);
    }
}

void MainComponent::processRenderStack(Component* root)
{
    TRACE_COMPONENT();
    if (root == nullptr)
        return;

    std::stack<Component*> componentsStack;
    componentsStack.push(root);

    while (!componentsStack.empty()) {
        auto* currentComponent = componentsStack.top();
        componentsStack.pop();

        if (!currentComponent->isVisible() || !currentComponent->isShowing())
            continue;

        if (auto nvcComp = dynamic_cast<NVGComponent *>(currentComponent)) {
            nvcComp->render(nvg);
        }

        for (auto& child : currentComponent->getChildren())
            componentsStack.push(child);
    }
}

void MainComponent::processRenderVector(Component* root)
{
    TRACE_COMPONENT();
    if (root == nullptr)
        return;

    std::vector<Component*> stack;
    stack.push_back(root);

    std::vector<NVGComponent*> nvgComponents; // Collect NVGComponent pointers

    while (!stack.empty()) {
        Component* current = stack.back();
        stack.pop_back();

        if (!current->isVisible() || !current->isShowing())
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
    for (NVGComponent* nvgComp : nvgComponents) {
        nvgComp->render(nvg);
    }
}

//==============================================================================


void MainComponent::resized()
{
    editor->setBounds(getBounds());
}
