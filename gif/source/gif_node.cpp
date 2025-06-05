#include "gif_node.hpp"

using namespace mm_gif;

gif_node::gif_node() : prev_(nullptr), next_(nullptr), size_(0), data_(nullptr) {}

gif_node::gif_node(std::size_t size)
{
    
    size_ = size;
    data_ = new uint8_t[size_];
    next_ = nullptr;
    prev_ = nullptr;
}

void gif_node::set_size(std::size_t size)
{
    size_ = size;
    data_ = new uint8_t[size_];
    next_ = nullptr;
    prev_ = nullptr;
}

gif_node::~gif_node()
{
    delete[] data_;
}

std::tuple<uint8_t*,std::size_t> gif_node::get_data()
{
    return std::tuple<uint8_t*,std::size_t>(data_,size_);
}

bool gif_node::load_image(void* data)
{
    if(data){
        std::memcpy(data_, data, size_);
        return true;
    }
    return false;
}

gif_node* gif_node::get_next()
{
    return next_;
}

gif_node* gif_node::get_prev()
{
    return prev_;
}

void gif_node::set_prev(gif_node* prev)
{
    prev_ = prev;
}

gif_node* gif_node::create_next(gif_node* next,std::size_t size)
{
    if(next) next_ = next;
    else next_ = new gif_node{size};
    next_->set_prev(this);
    return next_;
}

void gif_node::reset_list(gif_node* parent_node, CLEAR_MODE mode)
{   

    gif_node *next = next_;
    gif_node *prev = prev_;
    switch (mode)
    {
    case CLEAR_MODE::DEFAULT:
        if(next!=parent_node && next != nullptr) next->reset_list(parent_node,CLEAR_MODE::DEFAULT);
        break;
    case CLEAR_MODE::TWO_SIDED:
        if(next && next!=parent_node) next->reset_list(this,CLEAR_MODE::TWO_SIDED);
        if(prev && prev!=parent_node) prev->reset_list(this,CLEAR_MODE::TWO_SIDED);
        break;
    default:
        break;  
    }
    delete this;
}
