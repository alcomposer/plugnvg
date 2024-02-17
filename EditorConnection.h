//
// Created by alexmitchell on 16/02/24.
//

#pragma once
#include "NVGComponent.h"

class EditorNode;
class EditorConnection : public NVGComponent
{
public:
    EditorConnection(NVGComponent* parent, Component* node);

    void renderNVG(NVGcontext * nvg) override;

    Component* endNode = nullptr;

private:
    Component* node;
    bool isStraight = false;
};

