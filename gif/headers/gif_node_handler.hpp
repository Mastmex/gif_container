#pragma once
#include "gif_node.hpp"
#include "utility"

namespace mm_gif{

enum class LIST_STATUS : short {
    UNFINISHED=-1,
    CYCLED,
    PINGPONG
};

enum class LIST_DIRECTION : short {
    FORWARD,
    REVERSED
};

class gif_node_handler final
{
public:
    gif_node_handler();
    ~gif_node_handler();
    gif_node_handler(gif_node_handler&& other) noexcept;
    gif_node_handler& operator=(gif_node_handler&& other) noexcept;
    gif_node_handler(const gif_node_handler&) = delete;
    gif_node_handler& operator=(const gif_node_handler&) = delete;
    void create_cycle();
    void create_pingpong();
    void reverse();
    void add_data(void* data,std::size_t size);
    void set_default();
    std::tuple<uint8_t*,std::size_t,std::size_t> get_data(); //и продолжить дальше
private:
    gif_node* node_;
    gif_node* first_element_ = nullptr;
    bool first_set_ = false;
    bool forward_ = true;
    LIST_STATUS status_ = LIST_STATUS::UNFINISHED;
    LIST_DIRECTION direction_ = LIST_DIRECTION::FORWARD;
    std::size_t current_frame_ = 0;
    std::size_t maximum_frames_ = 0;
};


}
