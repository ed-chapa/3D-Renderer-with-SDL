#include "light.h"

light_t light = {
    .direction = {
        .x = 0.0,
        .y = 0.0,
        .z = 1.0
    }
};

uint32_t light_apply_intensity(uint32_t original_color, float percentage_facotr) {
    if (percentage_facotr < 0) percentage_facotr = 0;
    if (percentage_facotr > 1) percentage_facotr = 1;

    uint32_t a = (original_color & 0xFF000000);
    uint32_t r = (original_color & 0x00FF0000) * percentage_facotr;
    uint32_t g = (original_color & 0x0000FF00) * percentage_facotr;
    uint32_t b = (original_color & 0x000000FF) * percentage_facotr;

    int32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return new_color;
}