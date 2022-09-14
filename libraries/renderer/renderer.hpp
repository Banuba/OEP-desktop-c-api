#pragma once

#include <opengl/program.hpp>
#include "frame_surface_handler.hpp"

namespace bnb::render
{
    class renderer
    {
    public:
        renderer(int32_t width, int32_t height);

        void surface_change(int32_t width, int32_t height);

        void update_data(int texture_id);
        bool draw();

    private:
        bnb::oep::program m_program;
        frame_surface_handler m_frame_surface;

        int32_t m_width;
        int32_t m_height;
        int m_texture_id{0};

        std::atomic<bool> m_rendering = false;
        std::atomic<bool> m_texture_updated = false;

        std::atomic<bool> m_surface_changed = false;
    };
} // namespace bnb::render