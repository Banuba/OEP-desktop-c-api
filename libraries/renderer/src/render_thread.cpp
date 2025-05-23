#include "render_thread.hpp"
#include <chrono>

namespace bnb::render
{
    render_thread::render_thread(GLFWwindow* window, int32_t width, int32_t height)
        : m_window(window)
        , m_thread([this, width, height]() { thread_func(width, height); })
        , m_cancellation_flag(false) {}

    render_thread::~render_thread()
    {
        m_cancellation_flag = true;
        m_thread.join();
    }

    void render_thread::surface_changed(int32_t width, int32_t height)
    {
        if (m_renderer) {
            m_renderer->surface_change(width, height);
        }
    }

    void render_thread::update_data(int texture_id)
    {
        if (m_renderer)
            m_renderer->update_data(texture_id);
    }

    void render_thread::thread_func(int32_t width, int32_t height)
    {
        using namespace std::chrono_literals;

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1);

        m_renderer = std::make_unique<renderer>(width, height);

        while (!m_cancellation_flag) {
            if (m_renderer->draw()) {
                glfwSwapBuffers(m_window);
            } else {
                std::this_thread::sleep_for(1us);
            }
        }
        m_renderer.reset();
        glfwMakeContextCurrent(nullptr);
    }
} // bnb::render
