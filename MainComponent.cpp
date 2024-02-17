#include "MainComponent.h"
#include <stack>
#include <functional>
#include <vector>
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
    setOpaque(false);
    //if (auto *peer = getPeer()) {
    //    peer->setCurrentRenderingEngine(0);
    //}
    //glContext.setComponentPaintingEnabled(true);
    glContext.setOpenGLVersionRequired(OpenGLContext::openGL3_2);
    auto form = OpenGLPixelFormat(8,8,16,8);
    glContext.setPixelFormat(form);
        //glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE );
    glContext.attachTo(*this);

    editor = std::make_unique<Editor>(nullptr);
    addAndMakeVisible(editor.get());


    setSize (600, 600 * (9 / 16.0f));
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

void MainComponent::initialise()
{
    nvg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    if (!nvg)
        std::cout << "could not init nvg" << std::endl;
}

void MainComponent::shutdown()
{
}

void MainComponent::render()
{
    TRACE_COMPONENT();
    //glViewport(0, 0, getWidth(), getHeight());
    //OpenGLHelpers::clear(Colours::red);

    nvgBeginFrame(nvg, getWidth(), getHeight(), 1);
    //std::cout << "================== process render ====================" << std::endl;
    processRender(editor.get());
    nvgEndFrame(nvg);
}

void MainComponent::processRender(NVGComponent* c)
{
    TRACE_COMPONENT();
    if (c == nullptr)
        return;

    if (!c->isVisible() || !c->isShowing())
        return;
    //if (NVGComponent* nvcComp = dynamic_cast<NVGComponent*>(c)) {
        //std::cout << "rendering: " << c->getName() << std::endl;
        c->render(nvg);
    //}
    if (c->getNumChildComponents() > 0) {
        for (auto& child: c->children)
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
        Component* currentComponent = componentsStack.top();
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
void MainComponent::paint (juce::Graphics& g)
{
}

void MainComponent::resized()
{
    editor->setBounds(getBounds());
}
