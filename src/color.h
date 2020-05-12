#pragma once

#include <stdint.h>

/**
 * @brief Considering that we want n colors, generate the ith
 * @param i The index of the current color
 * @param n The total number of colors
 * @return A RGB color
 */
uint32_t generate_color(int i, int n);
