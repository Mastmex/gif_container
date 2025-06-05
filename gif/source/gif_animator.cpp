#include "gif_animator.hpp"
#include <chrono>


using namespace mm_gif;

gif_animator::gif_animator(gif_node_handler &&hndl, uint64_t frame_delay) :   handler_(std::move(hndl)),
                                                        frame_delay_ms_(frame_delay), playing_(false), current_frame_num_(0) {}


gif_animator::gif_animator(gif_animator&& other) noexcept : handler_(std::exchange(other.handler_,gif_node_handler())),
                                                            frame_delay_ms_(std::exchange(other.frame_delay_ms_,100)),
                                                            last_update_called_(std::exchange(other.last_update_called_,0)),
                                                            playing_(std::exchange(other.playing_,false)),
                                                            current_frame_num_(std::exchange(other.current_frame_num_,0)),
                                                            current_frame_(std::exchange(other.current_frame_,std::tuple<uint8_t*,std::size_t>())),
                                                            on_frame_callback_(std::exchange(other.on_frame_callback_,Frame_callback{})) {}


gif_animator& gif_animator::operator=(gif_animator&& other) noexcept
{
    if (this!=&other)
    {
        handler_ = std::exchange(other.handler_,gif_node_handler());
        frame_delay_ms_ = std::exchange(other.frame_delay_ms_,100);
        playing_ = std::exchange(other.playing_,false);
        current_frame_num_ = std::exchange(other.current_frame_num_,0);
        current_frame_ = std::exchange(other.current_frame_,std::tuple<uint8_t*,std::size_t>());
        on_frame_callback_ = std::exchange(other.on_frame_callback_,Frame_callback{});
    }
    return *this;
}

bool gif_animator::should_render() const
{
    return render_pending_;
}

void gif_animator::toggle()
{
    playing_=!playing_;
}

void gif_animator::update(uint64_t current_time_ms)
{
    if(current_time_ms-last_update_called_>=frame_delay_ms_ && playing_)
    {
        last_update_called_ = current_time_ms;
        auto [t_ptr,t_size,t_fr] = handler_.get_data();
        current_frame_num_ = t_fr;
        current_frame_ = {t_ptr,t_size};
        render_pending_=true;
        callback(current_frame_num_);
    }
    else render_pending_ = false;
}

std::tuple<uint8_t*,std::size_t> gif_animator::get_image() const
{
    return current_frame_;
}

void gif_animator::set_on_frame(Frame_callback cb)
{
    on_frame_callback_ = std::move(cb);
}

void gif_animator::callback(std::size_t frame)
{
    if(on_frame_callback_) on_frame_callback_(frame);
}