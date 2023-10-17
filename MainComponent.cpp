#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 600);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
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

void MainComponent::resized()
{
}
