#pragma once
#include <iostream>

#ifdef DEBUG
#define debugcout std::cout
#else
#define debugcout 0 && std::cout
#endif // DEBUG