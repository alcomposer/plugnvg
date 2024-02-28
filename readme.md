Test for using nanovg for drawing the plugdata interface.
( nanovg fork from @rgb2hsv )

Only tested on Linux at this point

Zooming does not work correctly yet

* Middle mouse to drag around
* Double click to add a node
* Drag from iolet to another iolet to make a connection

To build:
```
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make 
```

App will be in: `plugnvg/build/plugnvg_artefacts/Release/`

![plugnvg-example.png](plugnvg-example.png)