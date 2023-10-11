cmake_minimum_required(VERSION 3.12)
project(SimpleJUCEProject)

# Include JUCE
add_subdirectory(../JUCE)

# Your JUCE Application
add_executable(SimpleJUCEProject
    main.cpp
)

# Link JUCE libraries
target_link_libraries(SimpleJUCEProject
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
)

# Set the C++ standard
target_compile_features(SimpleJUCEProject PRIVATE cxx_std_17)