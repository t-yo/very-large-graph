#include "color.h"

#include <math.h>

uint32_t generate_color(int i, int n)
{
    double h = 360.0 * i / n;
    double s = 1;
    double l = 0.5;

    double c = (1 - fabs(2 * l - 1)) * s;
    double hprime = h / 60;
    double x = c * (1 - fabs(fmod(hprime, 2) - 1));
    double r1 = 0;
    double g1 = 0;
    double b1 = 0;
    int hprimeceil = ceil(hprime);
    switch (hprimeceil)
    {
        case 1:
            r1 = c;
            g1 = x;
            break;
        case 2:
            r1 = x;
            g1 = c;
            break;
        case 3:
            g1 = c;
            b1 = x;
            break;
        case 4:
            g1 = x;
            b1 = c;
            break;
        case 5:
            r1 = x;
            b1 = c;
            break;
        default:
            r1 = c;
            b1 = x;
            break;
    }
    double m = l - c / 2;
    double r = r1 + m;
    double g = g1 + m;
    double b = b1 + m;

    uint32_t uir = r * 255;
    uint32_t uig = g * 255;
    uint32_t uib = b * 255;
    return (uir << 16u) + (uig << 8u) + uib;
}
