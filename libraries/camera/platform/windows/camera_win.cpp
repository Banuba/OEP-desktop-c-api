#include "camera_win.hpp"

#include "camera_device.hpp"
#include <iostream>

struct bnb::camera_win::impl
{
    impl()
    {
        ComPtr<IMFMediaSource> ptrMediaSource;
        HRESULT hr = CreateVideoDeviceSource(&ptrMediaSource);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to crate media source");
        }
        wrapped = std::make_unique<VideoCaptureDeviceMFWin>(ptrMediaSource);
    }
    std::unique_ptr<VideoCaptureDeviceMFWin> wrapped = nullptr;
};


bnb::camera_win::camera_win(const camera_base::push_frame_cb_t& cb)
    : camera_base(cb)
    , m_impl(std::make_unique<impl>())
{
    constexpr auto camera_width = 1280;
    constexpr auto camera_hight = 720;
    constexpr auto frames_per_second = 30;
    m_impl->wrapped->Init();
    m_impl->wrapped->SetCallback([this, camera_width, camera_hight](std::shared_ptr<ScopedBufferLock> lock) {
        bnb_image_format_t format;
        format.orientation = BNB_DEG_0;
        format.face_orientation = 0;
        format.require_mirroring = true;
        format.width = camera_width;
        format.height = camera_hight;

        auto data = static_cast<uint8_t*>(lock->data());
        auto surface_stride = lock->pitch();
        auto y_plane_size = format.width * format.height;
        // see nv12_image.hpp
        auto img = std::make_shared<nv12_image>(
            color_plane(data, [lock](color_plane_data_t*) { /* DO NOTHING */ }), surface_stride,
            color_plane(data + y_plane_size, [lock](color_plane_data_t*) { /* DO NOTHING */ }), surface_stride,
            format);

        if (m_push_frame_cb) {
            m_push_frame_cb(std::move(img));
        }
    });
    m_impl->wrapped->AllocateAndStart(camera_width, camera_hight, frames_per_second);
}

bnb::camera_win::~camera_win()
{
    m_impl->wrapped->Stop();
}

void bnb::camera_win::set_device_by_index(uint32_t index)
{
    std::cout << "[Camera Win]: "
        << "Only default camera device supported";
}
void bnb::camera_win::set_device_by_id(const std::string& device_id)
{
    std::cout << "[Camera Win]: " <<
        "Only default camera device supported";
}

void bnb::camera_win::start()
{
    std::cout << "[Camera Win]: "
        << "Camera starts in constructor";
}

bnb::camera_sptr bnb::create_camera_device(camera_base::push_frame_cb_t cb, size_t index)
{
    return std::make_shared<bnb::camera_win>(cb);
}
