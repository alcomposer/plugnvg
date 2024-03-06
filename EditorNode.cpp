//
// Created by alexmitchell on 16/02/24.
//
#include "EditorNode.h"
#include "EditorNodeCanvas.h"
#include "GraphStateUpdater.h"

void EditorNode::mouseDown(MouseEvent const& e) {
    if (e.mods.isMiddleButtonDown()) {
        getParentComponent()->mouseDown(e);
        setMouseCursor(MouseCursor::DraggingHandCursor);
        return;
    }
    auto cnv = findParentComponentOfClass<EditorNodeCanvas>();
    cnv->setSingleSelected(this);
    mouseDownPos = e.getScreenPosition();
}

void EditorNode::mouseDrag(MouseEvent const &e) {
    //if (e.originalComponent != this)
    //    return;

    if (!e.mods.isLeftButtonDown()) {
        getParentComponent()->mouseDrag(e.getEventRelativeTo(getParentComponent()));
        return;
    }

    int offset = 0;

    if (e.originalComponent == this) {
        auto cnv = findParentComponentOfClass<EditorNodeCanvas>();
        //if (cnv->selectedComponents.getNumSelected() >= 2) {
            for (auto &node: cnv->selectedComponents) {
                if (node != this) {
                    node->mouseDownPos = mouseDownPos;
                    node->mouseDrag(e.getEventRelativeTo(getParentComponent()));

                }
            }
        //}
    }

    //dragDelta =  e.getScreenPosition() - mouseDownPos;
    //std::cout << "pos: " << pos.toString() << std::endl;
    //setTopLeftPosition(pos - dragDelta);
    pos += e.getScreenPosition() - mouseDownPos;
    GraphStateUpdater::getInstance().queueUpdate(this, [this]() {
        setTopLeftPosition(pos);
    });
    //setTopLeftPosition(pos);
    mouseDownPos = e.getScreenPosition();
    //dragDelta = mouseDownPos - e.getOffsetFromDragStart();
}