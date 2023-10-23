#include "qlib_test.h"

int main(void) {
    smoke_test_all();
    test_qremap_value();
    test_initialize_qkeys();
    return 0;
}