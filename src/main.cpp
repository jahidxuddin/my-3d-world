#include "core/window.h"

int main(void) {
    Window *window = window_new("My 3D World", 1080, 600);
    if (!window) {
        return -1;
    }
    window_run(window);
    return 0;
}
