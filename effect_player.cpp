#include "effect_player.hpp"

#include <iostream>
#include <thread>
#include <optional>
#include <iostream>
#include <array>
#include <algorithm>

namespace  {
    void check_error(bnb_error* e)
    {
        if (e) {
            std::string msg = bnb_error_get_message(e);
            bnb_error_destroy(e);
            throw std::runtime_error(msg);
        }
    }
    
    using planes_holder_t = std::array<bnb::oep::interfaces::pixel_buffer::plane_sptr, 3>;
    
    auto planes_holder(pixel_buffer_sptr image) {
        auto holder = new planes_holder_t;
        for (int i = 0; i < image->get_plane_count(); ++i) {
            holder->at(i) = image->get_base_sptr_of_plane(i);
        }
        return holder;
    }
    void planes_holder_release(void* holder) {
        delete reinterpret_cast<planes_holder_t*>(holder);
    }
};

namespace bnb::oep
{
    /* effect_player::effect_player CONSTRUCTOR */
    effect_player::effect_player(const std::vector<std::string>& path_to_resources, const std::string& client_token)
    {
        bnb_error* error = nullptr;
        std::unique_ptr<const char*[]> res_paths = std::make_unique<const char*[]>(path_to_resources.size() + 1);
        std::transform(path_to_resources.begin(), path_to_resources.end(), res_paths.get(), [](const auto& s) { return s.c_str(); });
        res_paths.get()[path_to_resources.size()] = nullptr;
        m_utility = bnb_utility_manager_init(res_paths.get(), client_token.c_str(), nullptr);

        // This particular example relies on OpenGL, so it should be explicitly requested
        bnb_effect_player_set_render_backend(bnb_render_backend_opengl, &error);

        bnb_effect_player_configuration_t ep_cfg{1, 1, bnb_nn_mode_enable, bnb_good, false, false};
        m_ep = bnb_effect_player_create(&ep_cfg, nullptr);
        if (m_ep == nullptr) {
            throw std::runtime_error("Failed to create effect player holder.");
        }
        
        auto config = bnb_processor_configuration_create(nullptr);
        
        bnb_processor_configuration_set_use_future_filter(config, false, nullptr);
        m_fp = bnb_frame_processor_create_realtime_processor(bnb_realtime_processor_mode_sync, config, &error);
        check_error(error);
        
        bnb_effect_player_set_frame_processor(m_ep, m_fp, &error);
        check_error(error);
        
        bnb_processor_configuration_destroy(config, nullptr);
    }

    /* effect_player::~effect_player */
    effect_player::~effect_player()
    {
        if (m_ep) {
            bnb_effect_player_destroy(m_ep, nullptr);
            m_ep = nullptr;
        }
        if (m_fp) {
            bnb_frame_processor_destroy(m_fp, nullptr);
            m_fp = nullptr;
        }
        if (m_utility) {
            bnb_utility_manager_release(m_utility, nullptr);
            m_utility = nullptr;
        }
    }

    /* effect_player::surface_created */
    void effect_player::surface_created(int32_t width, int32_t height)
    {
        bnb_effect_player_surface_created(m_ep, width, height, nullptr);
    }

    /* effect_player::surface_changed */
    void effect_player::surface_changed(int32_t width, int32_t height)
    {
        bnb_effect_player_surface_changed(m_ep, width, height, nullptr);
        effect_manager_holder_t* em = bnb_effect_player_get_effect_manager(m_ep, nullptr);
        bnb_effect_manager_set_effect_size(em, width, height, nullptr);
    }

    /* effect_player::surface_destroyed */
    void effect_player::surface_destroyed()
    {
        bnb_effect_player_surface_destroyed(m_ep, nullptr);
    }

    /* effect_player::load_effect */
    bool effect_player::load_effect(const std::string& effect)
    {
        if (auto e_manager = bnb_effect_player_get_effect_manager(m_ep, nullptr)) {
            bnb_effect_manager_load_effect(e_manager, effect.c_str(), nullptr);
            return true;
        }
        std::cout << "[Error] effect manager not initialized" << std::endl;
        return false;
    }

    /* effect_player::call_js_method */
    bool effect_player::call_js_method(const std::string& method, const std::string& param)
    {
        if (auto e_manager = bnb_effect_player_get_effect_manager(m_ep, nullptr)) {
            if (auto effect = bnb_effect_manager_get_current_effect(e_manager, nullptr)) {
                bnb_effect_call_js_method(effect, method.c_str(), param.c_str(), nullptr);
            } else {
                std::cout << "[Error] effect not loaded" << std::endl;
                return false;
            }
        } else {
            std::cout << "[Error] effect manager not initialized" << std::endl;
            return false;
        }
        return true;
    }

    /* effect_player::pause */
    void effect_player::pause()
    {
        bnb_effect_player_playback_pause(m_ep, nullptr);
    }

    /* effect_player::resume */
    void effect_player::resume()
    {
        bnb_effect_player_playback_play(m_ep, nullptr);
    }

    /* effect_player::push_frame */
    void effect_player::push_frame(pixel_buffer_sptr image, bnb::oep::interfaces::rotation image_orientation, bool require_mirroring)
    {
        full_image_holder_t * bnb_image {nullptr};
        
        bnb_error* error{nullptr};
        bnb_yuv_color_range_t range;
        bnb_yuv_color_space_t space;

        using ns = bnb::oep::interfaces::image_format;
        auto bnb_image_format = make_bnb_image_format(image, image_orientation);
        switch (image->get_image_format()) {
            case ns::bpc8_rgb:
            case ns::bpc8_bgr:
            case ns::bpc8_rgba:
            case ns::bpc8_bgra:
            case ns::bpc8_argb:
                bnb_image = bnb_full_image_from_bpc8_img_no_copy(
                    bnb_image_format,
                    make_bnb_pixel_format(image),
                    image->get_base_sptr().get(),
                    image->get_bytes_per_row(),
                    &planes_holder_release,
                    planes_holder(image),
                    &error);
                break;
                
                
            case ns::nv12_bt601_full:
                range = bnb_yuv_full_range;
                space = bnb_bt601;
            case ns::nv12_bt601_video:
                range = bnb_yuv_video_range;
                space = bnb_bt601;
            case ns::nv12_bt709_full:
                range = bnb_yuv_full_range;
                space = bnb_bt709;
            case ns::nv12_bt709_video:
                range = bnb_yuv_video_range;
                space = bnb_bt709;
            
                bnb_image = bnb_full_image_from_yuv_nv12_img_no_copy_ex(
                    &bnb_image_format,
                    range,
                    space,
                    image->get_base_sptr_of_plane(0).get(),
                    image->get_bytes_per_row_of_plane(0),
                    image->get_base_sptr_of_plane(1).get(),
                    image->get_bytes_per_row_of_plane(1),
                    &planes_holder_release,
                    planes_holder(image),
                    &error);
                break;
                
                
            case ns::i420_bt601_full:
                range = bnb_yuv_full_range;
                space = bnb_bt601;
            case ns::i420_bt601_video:
                range = bnb_yuv_video_range;
                space = bnb_bt601;
            case ns::i420_bt709_full:
                range = bnb_yuv_full_range;
                space = bnb_bt709;
            case ns::i420_bt709_video:
                range = bnb_yuv_video_range;
                space = bnb_bt709;
            
                bnb_image = bnb_full_image_from_yuv_i420_img_no_copy_ex(
                    &bnb_image_format,
                    range,
                    space,
                    image->get_base_sptr_of_plane(0).get(),
                    image->get_bytes_per_row_of_plane(0),
                    1,
                    image->get_base_sptr_of_plane(1).get(),
                    image->get_bytes_per_row_of_plane(1),
                    1,
                    image->get_base_sptr_of_plane(2).get(),
                    image->get_bytes_per_row_of_plane(2),
                    1,
                    &planes_holder_release,
                    planes_holder(image),
                    &error);
                break;
            default:
                break;
        }

        if (!bnb_image) {
            throw std::runtime_error("no image was created");
        }

        auto fd = bnb_frame_data_init(&error);
        check_error(error);
        
        bnb_frame_data_add_full_img(fd, bnb_image, &error);
        check_error(error);
        
        bnb_frame_processor_push(m_fp, fd, &error);
        check_error(error);

        bnb_frame_data_release(fd, nullptr);
        bnb_full_image_release(bnb_image, nullptr);
    }

    /* effect_player::draw */
    int64_t effect_player::draw()
    {
        bnb_error * error{nullptr};
        int64_t ret = -1;
         
        auto result = bnb_frame_processor_pop(m_fp, &error);
        check_error(error);
        if (result.status != bnb_processor_status_ok) {
            bnb_frame_data_release(result.frame_data, nullptr);
            return -1;
        }
        
        ret = bnb_effect_player_draw_with_external_frame_data(m_ep, result.frame_data, &error);
    
        bnb_frame_data_release(result.frame_data, nullptr);
        
        check_error(error);
        
        return ret;
    }

    /* effect_player::make_bnb_image_format */
    bnb_image_format_t effect_player::make_bnb_image_format(pixel_buffer_sptr image, interfaces::rotation orientation)
    {
        bnb_image_orientation_t camera_orient {BNB_DEG_0};
        using ns = bnb::oep::interfaces::rotation;
        switch (orientation) {
            case ns::deg0:
                camera_orient = BNB_DEG_0;
                break;
            case ns::deg90:
                camera_orient = BNB_DEG_90;
                break;
            case ns::deg180:
                camera_orient = BNB_DEG_180;
                break;
            case ns::deg270:
                camera_orient = BNB_DEG_270;
                break;
        }
        return {static_cast<uint32_t>(image->get_width()), static_cast<uint32_t>(image->get_height()), camera_orient, false, 0};
    }

    /* effect_player::make_bnb_pixel_format */
    bnb_pixel_format_t effect_player::make_bnb_pixel_format(pixel_buffer_sptr image)
    {
        bnb_pixel_format_t fmt {BNB_RGB};
        using ns = bnb::oep::interfaces::image_format;
        switch (image->get_image_format()) {
            case ns::bpc8_rgb:
                break;
            case ns::bpc8_bgr:
                fmt = BNB_BGR;
                break;
            case ns::bpc8_rgba:
                fmt = BNB_RGBA;
                break;
            case ns::bpc8_bgra:
                fmt = BNB_BGRA;
                break;
            case ns::bpc8_argb:
                fmt = BNB_ARGB;
                break;
            default:
                break;
        }
        return fmt;
    }
    
    void effect_player::eval_js(const std::string& script, oep_eval_js_result_cb result_callback)
    {
    
    }

    void effect_player::stop()
    {
    
    }

} /* namespace bnb::oep */
