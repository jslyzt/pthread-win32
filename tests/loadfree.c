#include "test.h"

int main() {
    HINSTANCE hinst;

    assert((hinst = LoadLibrary("pthread")) != (HINSTANCE) 0);

    Sleep(100);

    FreeLibrary(hinst);
    return 0;
}

