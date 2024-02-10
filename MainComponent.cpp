#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    //auto* mainWindow = findParentComponentOfClass<juce::DocumentWindow>();
    //resizableBorderComponent = std::make_unique<juce::ResizableBorderComponent>(mainWindow, nullptr);
    //resizableBorderComponent->setAlwaysOnTop(true);
    ////resized();
    //mainWindow->Component::addAndMakeVisible(resizableBorderComponent.get());
    setSize (600, 600 * (9 / 16.0f));
}

void MainComponent::drawCheckerboard(juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::grey);
    auto x_size = getWidth() / 10.0f;
    auto y_size = getHeight() / 10.0f;
    //auto x_size = 40;
    //auto y_size = 40;
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            auto x = x_size * i;
            auto y = y_size * j;

            // Draw a rectangle at the current position
            if ((i + j) % 2 == 0)
            {
                g.setColour(juce::Colours::black);
            }
            else
            {
                g.setColour(juce::Colours::white);
            }
            g.fillRect(x, y, x_size, y_size);
        }
    }
}

void MainComponent::drawRectangle(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::red);
    auto width = getWidth();
    auto height = getWidth() * (9 / 16.0f);
    g.drawRect(juce::Rectangle<int>(0, 0, width, height));

    auto count = 30;

    auto x_size = width / (float)count;
    auto y_size = height / (float)count;
    for (int i = 1; i < count; i++) {
        auto linePosH = i * x_size;
        g.drawLine(linePosH, 0, linePosH, height);
        auto linePosV = i * y_size;
        g.drawLine(0, linePosV, width, linePosV);
    }

    auto boxCount = count - 1;
    for (int i = 0; i <= boxCount; i++) {
        for (int j = 0; j <= boxCount; j++) {
            auto x = x_size * i;
            auto y = y_size * j;

            // Draw a rectangle at the current position
            if ((i == 0 && j == 0) || (i == boxCount && j == boxCount) || (i == boxCount && j == 0) || (i == 0 && j == boxCount))
                g.fillRect(x, y, x_size, y_size);
        }
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //drawCheckerboard(g);
    drawRectangle(g);
}

void MainComponent::resized()
{
    //resizableBorderComponent->setBounds(getLocalBounds());
}
