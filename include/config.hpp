#ifndef CONFIG_HPP
#define CONFIG_HPP

#define DEBUG

#ifdef DEBUG
#include <iostream>
#define OUTPUT_IF_DEBUG_(x)  std::cout << x << std::endl
#else
#define OUTPUT_IF_DEBUG_(x)
#endif 

#endif
