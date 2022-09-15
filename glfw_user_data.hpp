#pragma once

#include <bnb/effect_player.h>
#include "libraries/camera/camera.hpp"
#include "libraries/utils/glfw_window.hpp"
#include "libraries/renderer/renderer.hpp"

#include <string>
#include <memory>

namespace bnb
{
    class glfw_user_data
    {
    public:
        glfw_user_data(
            offscreen_effect_player_sptr oep,
            renderer_sptr render_target,
            camera_sptr& camera,
            bnb::camera::push_frame_cb_t push_frame_cb)
            : m_oep(oep)
            , m_camera(camera)
            , m_render_target(render_target)
            , m_push_frame_cb(push_frame_cb)
        {
        }

        ~glfw_user_data()
        {
        }

        offscreen_effect_player_sptr oep()
        {
            return m_oep.lock();
        }

        renderer_sptr render_target()
        {
            return m_render_target.lock();
        }

        camera_sptr& camera_ptr()
        {
            return m_camera;
        }

        bnb::camera::push_frame_cb_t push_frame_cb()
        {
            return m_push_frame_cb;
        }
    private:
        std::weak_ptr<offscreen_effect_player_sptr::element_type> m_oep;
        camera_sptr& m_camera;
        renderer_wptr m_render_target;
        bnb::camera::push_frame_cb_t m_push_frame_cb;
    };
} // namespace viewer
