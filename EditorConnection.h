//
// Created by alexmitchell on 16/02/24.
//

#pragma once
#include "NVGComponent.h"


class EditorNode;
class EditorNodeIolet;
class EditorConnection : public NVGComponent
{
public:

    enum CableType {Data, Signal, MCSignal};

    EditorConnection(EditorNodeIolet* node);

    void renderNVG(NVGcontext* nvg) override;

    Component* endNode = nullptr;

private:
    EditorNodeIolet* node;
    bool isStraight = false;
    CableType cableType = Data;
};

