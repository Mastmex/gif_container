#include "gif_node_handler.hpp"

using namespace mm_gif;

gif_node_handler::gif_node_handler()
{
    node_ = new gif_node;
    first_element_ = node_;
}
gif_node_handler::~gif_node_handler()
{
    if (status_ == LIST_STATUS::CYCLED)
        if (node_)
            node_->reset_list(node_, CLEAR_MODE::DEFAULT);
    if (status_ == LIST_STATUS::PINGPONG)
        if (node_)
            node_->reset_list(node_, CLEAR_MODE::TWO_SIDED);
    if (status_ == LIST_STATUS::UNFINISHED)
    {
        if (node_)
            node_->reset_list(node_, CLEAR_MODE::TWO_SIDED);
    }
    node_ = nullptr;
    first_element_ = nullptr;
}
gif_node_handler::gif_node_handler(gif_node_handler &&other) noexcept : node_(std::exchange(other.node_, nullptr)),
                                                                        first_element_(std::exchange(other.first_element_, nullptr)),
                                                                        first_set_(std::exchange(other.first_set_, false)),
                                                                        forward_(std::exchange(other.forward_, true)),
                                                                        status_(std::exchange(other.status_, LIST_STATUS::UNFINISHED)),
                                                                        direction_(std::exchange(other.direction_,LIST_DIRECTION::FORWARD)),
                                                                        current_frame_(std::exchange(other.current_frame_,0)),
                                                                        maximum_frames_(std::exchange(other.maximum_frames_,0)) {}

gif_node_handler &gif_node_handler::operator=(gif_node_handler &&other) noexcept
{
    if (this != &other)
    {

        if (node_)
        {
            if (status_ == LIST_STATUS::CYCLED) node_->reset_list(node_, CLEAR_MODE::DEFAULT);
            if (status_ == LIST_STATUS::PINGPONG) node_->reset_list(node_, CLEAR_MODE::TWO_SIDED);
            if (status_ == LIST_STATUS::UNFINISHED) node_->reset_list(node_, CLEAR_MODE::DEFAULT);
        }
        node_ = std::exchange(other.node_, nullptr);
        first_element_ = std::exchange(other.first_element_, nullptr);
        first_set_ = std::exchange(other.first_set_, false);
        forward_ = std::exchange(other.forward_, true);
        status_ = std::exchange(other.status_, LIST_STATUS::UNFINISHED);
        direction_ = std::exchange(other.direction_,LIST_DIRECTION::FORWARD);
        current_frame_ = std::exchange(other.current_frame_,0);
        maximum_frames_ = std::exchange(other.maximum_frames_,0);
    }
    return *this;
}
void gif_node_handler::create_cycle()
{
    if (first_element_)
        node_->create_next(first_element_, NULL);
    status_ = LIST_STATUS::CYCLED;
    node_=first_element_;
    current_frame_ = 0;
}

void gif_node_handler::create_pingpong()
{
    status_ = LIST_STATUS::PINGPONG;
    if(direction_ == LIST_DIRECTION::FORWARD) {node_ = first_element_; current_frame_=0;}
    else {first_element_ = node_; current_frame_ = maximum_frames_;}
}

void gif_node_handler::add_data(void *data, std::size_t size)
{
    if (!first_set_)
    {
        node_->set_size(size);
        first_set_ = true;
        current_frame_ = 1;
        maximum_frames_ = 1;
    }
    else
    {
        node_ = node_->create_next(nullptr, size);
        ++current_frame_;
        ++maximum_frames_;
    }
    node_->load_image(data);
}
std::tuple<uint8_t *, std::size_t,std::size_t> gif_node_handler::get_data() // и продолжить дальше
{
    if (status_ == LIST_STATUS::CYCLED)
    {
        auto [data, size] = node_->get_data();
        auto temp_frame =  current_frame_;
        if(!node_->get_next()) return std::tuple<uint8_t *, std::size_t,std::size_t>(data, size,temp_frame+1);
        if (direction_ == LIST_DIRECTION::FORWARD)
        {
            node_ = node_->get_next();
            current_frame_ = (current_frame_ + 1) % maximum_frames_;
        }
        if (direction_ == LIST_DIRECTION::REVERSED)
        {
            node_ = node_->get_prev();
            current_frame_ = (current_frame_ - 1 + maximum_frames_) % maximum_frames_;
        }
        return std::tuple<uint8_t *, std::size_t,std::size_t>(data, size,temp_frame+1);

    }
    else if (status_ == LIST_STATUS::PINGPONG)
    {
        gif_node *temp = nullptr;
        auto [data, size] = node_->get_data();
        auto temp_frame =  current_frame_;
        auto func = [this,&temp](){
            if(forward_ == (direction_ == LIST_DIRECTION::FORWARD)) 
            {
                temp = node_->get_next();
                current_frame_ = (current_frame_ + 1) % maximum_frames_;
            }
            else
            {
                temp = node_->get_prev();
                current_frame_ = (current_frame_ - 1 + maximum_frames_) % maximum_frames_;
            }
        };
        func();
        if (!temp)
        {
            current_frame_ = temp_frame;
            forward_ = !forward_;
            func();
        }
        node_ = temp;
        return std::tuple<uint8_t *, std::size_t,std::size_t>(data, size,temp_frame+1);
    }
    else
        return std::tuple<uint8_t *, std::size_t,std::size_t>(nullptr, 0, 0);
}
void gif_node_handler::set_default()
{
    node_ = first_element_;
    if (direction_ == LIST_DIRECTION::FORWARD || status_ == LIST_STATUS::CYCLED) current_frame_ = 0;
    else current_frame_ = maximum_frames_;
}

void gif_node_handler::reverse()
{
    if (direction_ == LIST_DIRECTION::FORWARD)
        direction_ = LIST_DIRECTION::REVERSED;
    else
        direction_ = LIST_DIRECTION::FORWARD;
}