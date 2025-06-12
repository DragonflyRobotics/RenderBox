#include <X11/Xlib.h>
#include <cstddef>
#include <stdlib.h>
#include <stdio.h>

struct X11_Handle {
    Display* display;
    Window window;
    GC graphics_context;
    XEvent events;
    Pixmap backbuffer;
};

X11_Handle initializeX11() {
    // This function is intentionally left empty.
    // The X11 initialization code is already provided in the main.cpp snippet.
	//Open Display
	Display *di = XOpenDisplay(getenv("DISPLAY"));
	if (di == NULL) {
		printf("Couldn't open display.\n");
        return X11_Handle{};
	}

	//Create Window
	int const x = 0, y = 0, width = 640, height = 480, border_width = 1;
	int sc    = DefaultScreen(di);
	Window ro = DefaultRootWindow(di);
	Window wi = XCreateSimpleWindow(di, ro, x, y, width, height, border_width, 
                                BlackPixel(di, sc), WhitePixel(di, sc));
	XMapWindow(di, wi); //Make window visible
	XStoreName(di, wi, "Window sample"); // Set window title
	
	//Prepare the window for drawing
	GC gc = XCreateGC(di, ro, 0, NULL);

	//Select what events the window will listen to
	XSelectInput(di, wi, KeyPressMask | ExposureMask);
	XEvent ev;

    Pixmap buffer = XCreatePixmap(di, wi, 640, 480,
                                  DefaultDepth(di, sc));

    return X11_Handle {
        di,
        wi,
        gc,
        ev,
        buffer
    };
}

void drawpixel(X11_Handle handle, int x, int y, int color)
{
	XSetForeground(handle.display, handle.graphics_context, color);
	XDrawPoint(handle.display, handle.backbuffer, handle.graphics_context, x, y);
}

void destroyX11(X11_Handle handle) {
	XFreeGC(handle.display, handle.graphics_context);
	XDestroyWindow(handle.display, handle.window);
	XCloseDisplay(handle.display);
}
