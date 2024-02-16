#include "MainComponent.h"
//#include "NVGComponent.h"
//#include "Editor.h"
//==============================================================================
MainComponent::MainComponent()
{
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

    editor = std::make_unique<Editor>();
    addAndMakeVisible(editor.get());


    setSize (600, 600 * (9 / 16.0f));
    startTimerHz(60);
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
    glViewport(0, 0, getWidth(), getHeight());
    OpenGLHelpers::clear(Colours::red);

    nvgBeginFrame(nvg, getWidth(), getHeight(), 1);
    //std::cout << "process render" << std::endl;
    processRender(this);
    nvgEndFrame(nvg);
}

void MainComponent::processRender(Component* node)
{
    if (node != nullptr) {
        //std::cout << "rendering: " << node->getName() << std::endl;
        if (auto toRender = dynamic_cast<NVGComponent*>(node)) {
            toRender->render(nvg);
        }
        for (auto& child : node->getChildren()) {
            processRender(child);
        }
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
