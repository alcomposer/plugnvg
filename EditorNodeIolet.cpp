//
// Created by alexmitchell on 16/02/24.
//

#include "EditorNodeIolet.h"
#include "EditorConnection.h"
#include "EditorNodeCanvas.h"

void EditorNodeIolet::mouseDown(MouseEvent const& e)
{
    auto editor = findParentComponentOfClass<EditorNodeCanvas>();
    newConnection = new EditorConnection(this);
    editor->addConnection(newConnection);
}

void EditorNodeIolet::mouseDrag(MouseEvent const &e) {
    if (auto iolet = dynamic_cast<EditorNodeIolet*>(getTopLevelComponent()->getComponentAt(e.getScreenPosition() - getTopLevelComponent()->getScreenPosition()))) {
        if (iolet == this)
            return;
        if (foundIolet)
            foundIolet->isActive = false;

        iolet->isActive = true;
        foundIolet = iolet;
    } else if (foundIolet) {
        foundIolet->isActive = false;
        foundIolet = nullptr;
    }
}

void EditorNodeIolet::mouseUp(MouseEvent const& e)
{
    if (foundIolet) {
        std::cout << "found iolet" << std::endl;
        newConnection->endNode = foundIolet;
        foundIolet = nullptr;
    }
    if (newConnection && newConnection->endNode == nullptr) {
        std::cout << "connection is not complete, delete cable" << std::endl;
        if (auto editor = findParentComponentOfClass<EditorNodeCanvas>()) {
            editor->removeUnsuccessfulConnection();
            newConnection = nullptr;
        }
    }
}