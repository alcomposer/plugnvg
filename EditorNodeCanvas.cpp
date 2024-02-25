//
// Created by alexmitchell on 18/02/24.
//
#include "EditorNodeCanvas.h"
#include "MainComponent.h"

EditorNodeCanvas::EditorNodeCanvas()
{
    setName("node canvas");

    delta = Point<int>(-halfSize, -halfSize);


    addAndMakeVisible(&lasso);
    lasso.setAlwaysOnTop(true);

    selectedComponents.addChangeListener(this);

    setSize(canvasSize, canvasSize);
    setTopLeftPosition(-halfSize, -halfSize);

    //lock = findParentComponentOfClass<MainComponent>()->lock;
    //std::cout << "lock: " << lock << std::endl;

#if (TEST_1000 == 1)
    Random random;
    Range range = Range<int>(100000, 101000);

    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < 60; j++) {
            //auto pos = Point<int>(random.nextInt(range), random.nextInt(range));
            auto pos = Point<int>((i * 110) + 100000, (j * 40) + 100000);
            auto node = new EditorNode(pos);
            //node->setCentrePosition(pos);
            nodes.add(node);
            addAndMakeVisible(node);
        }
    }
#endif
}

void EditorNodeCanvas::mouseDrag(MouseEvent const &e) {
    //TRACE_COMPONENT();
    ScopedTryLock lock(findParentComponentOfClass<MainComponent>()->renderLock);
    if (e.mods.isMiddleButtonDown()) {
        setMouseCursor(MouseCursor::DraggingHandCursor);
        delta = currentPos + e.getOffsetFromDragStart();
    } else if (e.mods.isLeftButtonDown()) {
        lasso.dragLasso(e);
    }
}

void EditorNodeCanvas::removeConnection()
{
    while(true) {
        //std::cout << "trying to aquire lock" << std::endl;
        ScopedTryLock lock(findParentComponentOfClass<MainComponent>()->renderLock);
        if (lock.isLocked()){
            std::cout << "cons size: " << cons.size() << std::endl;
            cons.removeLast();
            return;
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
}