//
// Created by alexmitchell on 16/02/24.
//
#pragma once
#include <JuceHeader.h>

class EditorNode;
class EditorConnection;
class EditorNodeIolet : public Component
{
public:
    enum Iolet {Inlet, Outlet};

    EditorNodeIolet(EditorNode* parentNode, int index, Iolet type = Iolet::Outlet);

    void mouseEnter(MouseEvent const& e) override
    {
        isActive = true;
    }

    void mouseDown(MouseEvent const& e) override;
    void mouseUp(MouseEvent const& e) override;
    void mouseDrag(MouseEvent const& e) override;

    void mouseExit(MouseEvent const& e) override
    {
        isActive = false;
    }

    Iolet ioletType;
    Point<int> pos;
    int ioletIndex = 0;
    bool isActive = false;
    EditorNodeIolet* foundIolet = nullptr;

    SafePointer<EditorConnection> newConnection;
    bool addingNewConnection = false;
private:
    EditorNode* parentNode;
};

