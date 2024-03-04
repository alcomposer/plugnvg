//
// Created by alexmitchell on 16/02/24.
//

#include "EditorNodeIolet.h"
#include "EditorConnection.h"
#include "EditorNodeCanvas.h"
#include "EditorNode.h"

EditorNodeIolet::EditorNodeIolet(EditorNode* parentNode, int index, Iolet type)
    : ioletIndex(index)
    , ioletType(type)
    , parentNode(parentNode)
{
    static int number = 0;
    setName("iolet " + String(number++));
    setSize(20,20);
}

void EditorNodeIolet::mouseDown(MouseEvent const& e)
{
    auto cnv = findParentComponentOfClass<EditorNodeCanvas>();
    if (e.mods.isShiftDown()) {
        cnv->selectedComponents.addToSelection(parentNode);
        parentNode->isSelected = true;
        for (auto &selected: cnv->selectedComponents) {
            if (ioletType == Iolet::Outlet) {
                if (selected->outlets.size() > ioletIndex) {
                    auto outlet = selected->outlets[ioletIndex];
                    outlet->newConnection = new EditorConnection(outlet);
                    cnv->addConnection(outlet->newConnection);
                }
            } else {
                if (selected->inlets.size() > ioletIndex) {
                    auto inlet = selected->inlets[ioletIndex];
                    inlet->newConnection = new EditorConnection(inlet);
                    cnv->addConnection(inlet->newConnection);
                }
            }
            addingNewConnection = true;
        }
    } else {
        cnv->addConnection(new EditorConnection(this));
        addingNewConnection = true;
    }
}

void EditorNodeIolet::mouseDrag(MouseEvent const &e) {
    auto objectLayer = findParentComponentOfClass<EditorNodeCanvas>()->getObjectLayer();
    if (auto iolet = dynamic_cast<EditorNodeIolet*>(objectLayer->getComponentAt(e.getScreenPosition() - objectLayer->getScreenPosition()))) {
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
    auto cnv = findParentComponentOfClass<EditorNodeCanvas>();

    if (foundIolet) {
        std::cout << "found iolet" << std::endl;
        for (auto con : cnv->cons) {
            if (con->endNode == nullptr)
                con->endNode = foundIolet;
        }
        addingNewConnection = false;
        foundIolet = nullptr;
    }
    if (addingNewConnection) {
        std::cout << "connection is not complete, delete cable/s" << std::endl;
        cnv->removeUnsuccessfulConnections();
    }
}