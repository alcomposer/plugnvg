cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("libraries/JUCE")
#add_subdirectory("libraries/nanovg")
add_subdirectory("libraries/nanovg-dev")
include_directories("libraries/nanovg-dev")

add_subdirectory("libraries/melatonin_perfetto")

project(PLUGNVG VERSION 0.0.1)

add_compile_options(-fpermissive)

# Global compile options
add_compile_options(
        # Build with Multiple Processes on Visual Studio
        $<$<CXX_COMPILER_ID:MSVC>:/MP>
        # Set source and executable charsets to UTF-8. Required for building on CJK Windows.
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

juce_add_gui_app(plugnvg
    # VERSION ...                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    # COMPANY_NAME ...                  # Specify the name of the app's author
    PRODUCT_NAME "plugnvg")     # The name of the final executable, which can differ from the target name

target_sources(plugnvg
    PRIVATE
        Main.cpp
        MainComponent.cpp
        EditorNodeIolet.cpp
        EditorConnection.cpp
        EditorNode.cpp
        EditorNodeCanvas.cpp
        )

target_compile_definitions(plugnvg
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:plugnvg,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:plugnvg,JUCE_VERSION>"
        JUCE_ENABLE_GPL_MODE=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_XSHM=1
        JUCE_USE_XRENDER=1
        PERFETTO=0
        JUCE_FORCE_DEBUG=1
        #TEST_1000=1
        )

target_link_libraries(plugnvg
    PRIVATE
        # plugnvg data            # If we'd created a binary data target, we'd link to it here
        juce::juce_gui_extra
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_opengl
        #test::nanovg
        nanovg
        Melatonin::Perfetto
        )

juce_generate_juce_header(plugnvg)