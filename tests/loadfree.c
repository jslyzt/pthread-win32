#include "test.h"

int main() {
    HINSTANCE hinst = LoadLibrary("pthread");

    Sleep(100);

    FreeLibrary(hinst);
    return 0;
}

