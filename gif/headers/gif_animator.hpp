#pragma once
#include "gif_node_handler.hpp"
#include "IGifEventBinder.hpp"
#include <functional>

namespace mm_gif{
class gif_animator final : public IGifEventBinder
{
public:
    using Frame_callback = std::function<void(int)>;
    gif_animator() = delete;
    gif_animator(gif_node_handler &&hndl, uint64_t frame_delay);
    gif_animator(gif_animator&& other) noexcept;
    gif_animator& operator=(gif_animator&& other) noexcept;
    gif_animator(const gif_animator&) = delete;
    gif_animator& operator=(const gif_animator&) = delete;
    void update(uint64_t current_time_ms);
    void toggle();
    bool should_render() const;
    std::tuple<uint8_t*,std::size_t> get_image() const;
    void set_on_frame(Frame_callback cb) override;

private:
gif_node_handler handler_;
uint64_t frame_delay_ms_ = 100;
uint64_t last_update_called_=0;
bool playing_ = false;
bool render_pending_ = false;
std::size_t current_frame_num_ = 0;
std::tuple<uint8_t*,std::size_t> current_frame_;
Frame_callback on_frame_callback_;

void callback(std::size_t frame);
};



}

