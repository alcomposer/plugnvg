#include <JuceHeader.h>

class MainComponent : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightgrey);
    }
};

int main(int argc, char* argv[])
{
    juce::initialiseJuce_GUI();
    juce::ScopedJuceInitialiser_GUI libraryInitialiser;
    MainComponent mainWindow;
    juce::ComponentBuilder<MainComponent> builder;
    builder.setColour(juce::Colours::lightgrey);
    mainWindow.setSize(600, 400);
    mainWindow.setVisible(true);
    juce::shutdownJuce_GUI();
    return 0;
}