//
// Created by alexw on 6/03/2024.
//

#pragma once

#include <JuceHeader.h>
#include "EditorNode.h"

/*
 * This class allows node objects to store resize functions
 * Only the most recent function will be saved, calling updateGraphState will swap, run and clear the current state
 * This allows mouse events that set bounds to effectively be locked, yet we will always get the most recent event
 *
 * We do this because mouse events come through at the pointing devices polling rate & as such the events will come in
 * between the drawing callback
 */

class GraphStateUpdater
{
public:
    static GraphStateUpdater& getInstance()
    {
        static GraphStateUpdater instance;
        return instance;
    }

    GraphStateUpdater(){};

    /*
     * Add a function, which will be mapped to the node object. New functions (for the same node)
     * that come in before updateGraphState is called will be overwritten
     */
    void queueUpdate(void* node, std::function<void()> updateFunc)
    {
        auto& latestFunctionMap = activeFunctionMap.load() ? latestFunctionMapA : latestFunctionMapB;
        latestFunctionMap[node] = updateFunc;
    }

    /*
     * Swaps the active map, calls all functions (one for each object) in the map that is now offline, and clears it.
     */
    void updateGraphState()
    {
        TRACE_COMPONENT();
        // swap the map to store new functions to
        auto activeMap = activeFunctionMap.load();
        activeFunctionMap.store(!activeMap);

        // read the map that is now offline
        auto& processingFunctionMap = activeMap ? latestFunctionMapA : latestFunctionMapB;
        for (auto& pair : processingFunctionMap) {
            auto& func = pair.second;
            if (func) {
                func();
            }
        }

        // clear the function pointers as they have now been processed
        processingFunctionMap.clear();
    }
private:

    std::atomic<bool> activeFunctionMap = false;
    std::unordered_map<void*, std::function<void()>> latestFunctionMapA;
    std::unordered_map<void*, std::function<void()>> latestFunctionMapB;
};