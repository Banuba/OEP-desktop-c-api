#pragma once

#include <effect_player.h>

namespace bnb::interfaces {
    struct orient_format
    {
        bnb_image_orientation_t orientation;
        bool is_y_flip;
    };

} // bnb::interfaces