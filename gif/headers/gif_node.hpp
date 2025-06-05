#pragma once
    #include <cstdint>
#include <tuple>
#include <memory>
#include <cstring>

namespace mm_gif{

enum class CLEAR_MODE : short {
    DEFAULT,
    TWO_SIDED
};


class gif_node final
{
private:
    gif_node();
    gif_node(std::size_t size);
    ~gif_node();
    void set_size(std::size_t size);
    bool load_image(void* data);    //note: массив будет формата uint_8t (char) - принять размерность заполняемость данных во внимание
    std::tuple<uint8_t*,std::size_t> get_data(); // auto [data,size] = get_data();
    gif_node* create_next(gif_node* next, std::size_t size);
    gif_node* get_next();
    gif_node* get_prev();
    void set_prev(gif_node* prev);
    void reset_list(gif_node* parent_node, CLEAR_MODE mode);

    gif_node* prev_;
    gif_node* next_;
    std::size_t size_;
    uint8_t* data_;


    friend class gif_node_handler;
};

}


