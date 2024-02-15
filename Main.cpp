#include <JuceHeader.h>
#include "MainComponent.h"

class BorderBoundsConstrainer : public ComponentBoundsConstrainer {
  public:
    BorderBoundsConstrainer() : ComponentBoundsConstrainer() { }

    void checkBounds(Rectangle<int>& bounds, const Rectangle<int>& previous,
                             const Rectangle<int>& limits,
                             bool stretching_top, bool stretching_left,
                             bool stretching_bottom, bool stretching_right) override
    {
        std::cout << "======================checkint bounds" << std::endl;
        border_.subtractFrom(bounds);
        ComponentBoundsConstrainer::checkBounds(bounds, previous, limits,
                                          stretching_top, stretching_left,
                                          stretching_bottom, stretching_right);
        border_.addTo(bounds);
    }

    void setBorder(BorderSize<int> border) { border_ = border; }

  protected:
    BorderSize<int> border_;
};

//==============================================================================
class GuiAppApplication  : public juce::JUCEApplication {
public:
    //==============================================================================
    GuiAppApplication() {}

    // We inject these as compile definitions from the CMakeLists.txt
    // If you've enabled the juce header with `juce_generate_juce_header(<thisTarget>)`
    // you could `#include <JuceHeader.h>` and use `ProjectInfo::projectName` etc. instead.
    const juce::String getApplicationName() override { return JUCE_APPLICATION_NAME_STRING; }

    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }

    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise(const juce::String &commandLine) override {
        // This method is where you should put your application's initialisation code..
        juce::ignoreUnused(commandLine);

        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted(const juce::String &commandLine) override {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
        juce::ignoreUnused(commandLine);
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
class MainWindow    : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (juce::String name)
            : DocumentWindow ("non-native window resize test",
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setOpaque(false);
            auto useNativeTitlebar = false;
            setUsingNativeTitleBar (useNativeTitlebar);
            //setResizable(true, false);
            //setResizeLimits(500, 500, 500, 500);
            //getPeer()->forceNotResizable();
            setTitleBarHeight(0);
            setBackgroundColour(Colours::transparentBlack);
            //auto titlebarHeight = 37;
            //setTitleBarHeight(37);
            //std::cout << "title bar height is: " << getTitleBarHeight() << std::endl;
            constrainer = std::make_unique<BorderBoundsConstrainer>();
            //constrainer->setBorder(BorderSize<int>(36, 0, 0, 0));
            //mainConstrainer = std::make_unique<MainConstrainer>(this, constrainer.get());
            constrainer->setMinimumSize(500, (500) * (9 / 16.0f));
            //constrainer->set()
            auto AR = 16/9.0f;
            //auto w = 500;
            //auto h = (500 + 47) * AR;
            //auto newAR = w/h;
            constrainer->setFixedAspectRatio(AR);
            setConstrainer(constrainer.get());
            mainComponent = std::make_unique<MainComponent>();
            setContentOwned (mainComponent.get(), true);

            //setResizable(true, false);

            //if (!useNativeTitlebar) {
                border = std::make_unique<juce::ResizableBorderComponent>(this, constrainer.get());
                border->setAlpha(0.0f);
                border->setAlwaysOnTop(true);
                addAndMakeVisible(border.get());
            //}

            setVisible (true);
        }

        void resized() override
        {
            if (border)
                border->setBounds(getLocalBounds().withTop(-47));

            /*
            std::cout << "border: top: " << getBorderThickness().getTop()
                      << " bottom: " << getBorderThickness().getBottom()
                      << " left: " << getBorderThickness().getLeft()
                      << " right: " << getBorderThickness().getRight()
                      << std::endl;
            */
             DocumentWindow::resized();
        }

        //BorderSize<int> getContentComponentBorder() override
        //{
        //    return BorderSize<int>(36, 0, 0, 0);
        //}

        //BorderSize<int> getBorderThickness() override
        //{
        //    return BorderSize<int>(36, 0, 0, 0);
        //}

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        std::unique_ptr<BorderBoundsConstrainer> constrainer;
        std::unique_ptr<ResizableBorderComponent> border;
        //std::unique_ptr<MainConstrainer> mainConstrainer;
        std::unique_ptr<MainComponent> mainComponent;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (GuiAppApplication)
