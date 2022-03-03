#pragma once

#include <functional>
#include <atomic>
#include <string>
#include <vector>
#include <interfaces/pixel_buffer.hpp>

namespace bnb
{
    class camera;
}

using camera_sptr = std::shared_ptr<bnb::camera>;

namespace bnb
{

    class camera
    {
    public:
        struct camera_device_description
        {
            std::string localized_name;
        }; /* struct camera_device_description */

        using push_frame_cb_t = std::function<void(pixel_buffer_sptr)>;

    public:
        /**
         * Create camera
         *
         * @param cb callback for push frames
         * @param index camera device number
         *
         * @return shared pointer to the camera
         *
         * @example bnb::oep::interfaces::camera::create([](pixel_buffer_sptr image){}, 0)
         */
        static camera_sptr create(push_frame_cb_t cb, size_t index);

        explicit camera(push_frame_cb_t cb);
        ~camera();

        void set_device_by_index(uint32_t index);

        void set_device_by_id(const std::string& device_id);

        void start();

        const std::vector<camera::camera_device_description> get_connected_devices() const;

        size_t get_current_device_index() const;

    private:
        push_frame_cb_t m_push_frame_cb;
        std::vector<camera_device_description> m_connected_devices;
        size_t m_device_index{0};
        struct impl;
        std::unique_ptr<impl> m_impl;
    }; /* class camera */

} /* namespace bnb */
