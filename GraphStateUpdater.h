//
// Created by alexw on 6/03/2024.
//

#pragma once

#include <JuceHeader.h>
#include "EditorNode.h"

class GraphStateUpdater
{
public:
    static GraphStateUpdater& getInstance()
    {
        static GraphStateUpdater instance;
        return instance;
    }

    GraphStateUpdater(){};

    void queueUpdate(EditorNode* node, std::function<void()> updateFunc)
    {
        auto& latestFunctionMap = activeFunctionSet.load() ? latestFunctionMapB : latestFunctionMapA;
        latestFunctionMap[node] = updateFunc;
    }

    void updateGraphState()
    {
        activeFunctionSet.store(!activeFunctionSet.load());

        auto& processingFunctionMap = activeFunctionSet.load() ? latestFunctionMapB : latestFunctionMapA;
        for (auto& pair : processingFunctionMap) {
            auto& func = pair.second;
            if (func) {
                func();
            }
        }
        processingFunctionMap.clear();
    }
private:

    std::atomic<bool> activeFunctionSet = false;
    std::unordered_map<EditorNode*, std::function<void()>> latestFunctionMapA;
    std::unordered_map<EditorNode*, std::function<void()>> latestFunctionMapB;
};