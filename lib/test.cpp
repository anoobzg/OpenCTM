#include <iostream>
#include "openctm.h"

int main(int argc, char** argv) {
    std::cout << CTM_API_VERSION << std::endl;
    std::cout << ctmErrorString(CTM_BAD_FORMAT) << std::endl;
    return 0;
}