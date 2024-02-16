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

void EditorNodeIolet::mouseUp(MouseEvent const& e)
{
    if (foundIolet) {
        newConnection->endNode = foundIolet;
        foundIolet = nullptr;
    }
    if (newConnection->endNode == nullptr) {
        auto editor = findParentComponentOfClass<EditorNodeCanvas>();
        editor->removeConnection();
    }
}