#pragma once
#include <concepts>
#include <memory>
#include <unordered_map>
#include <filesystem>
#include "IGiffable.hpp"
#include "gif_animator.hpp"

namespace mm_gif{

template <typename T>
concept Giffable = std::derived_from<T,IGiffable>;

template <Giffable T>
class giffable_factory final
{
public:
    giffable_factory() {};
    std::unordered_map<std::string,mm_gif::gif_animator> process_data(const std::filesystem::path &file);
private:
std::unique_ptr<IGiffable> processor_;
void clear();
void reset();
void load_file(const std::filesystem::path &file);
uint8_t get_type(const std::string& tag);
uint64_t get_delay();
std::unordered_map<std::string,int> get_table();
};


template <Giffable T>
std::unordered_map<std::string,mm_gif::gif_animator> giffable_factory<T>::process_data(const std::filesystem::path &file)
{
    std::unordered_map<std::string,mm_gif::gif_animator> final_map;
    reset();
    load_file(file);
    auto table{get_table()};
    uint64_t delay = get_delay();
    for(auto&& row: table)
    {
        gif_node_handler hndl{};
        uint8_t type = get_type(row.first);
        for(uint32_t i = 0, last = row.second;i<last;i++)
        {
            auto [data,size] = processor_->get_next_frame(row.first,i);
            hndl.add_data(data,size);
        }
        switch (type)
        {
        case 0x00,0x01:
            hndl.create_cycle();
            break;
        case 0x02:
            hndl.create_pingpong();
            break;
        case 0x03:
            hndl.create_cycle();
            hndl.reverse();
            break;
        case 0x04:
            hndl.create_pingpong();
            hndl.reverse();
            break;
        default:
            break;
        }
        gif_animator anim{std::move(hndl),delay};
        final_map.insert(row.first,std::move(anim));
    }
    clear();
    return std::move(final_map);
}

template <Giffable T>
void giffable_factory<T>::clear()
{
    processor_.reset();
}

template <Giffable T>
void giffable_factory<T>::reset()
{
    processor_ = std::make_unique<T>();
}

template <Giffable T>
void giffable_factory<T>::load_file(const std::filesystem::path &file)
{
    processor_->load_image(file);
}

template <Giffable T>
std::unordered_map<std::string,int> giffable_factory<T>::get_table()
{
    return processor_->get_table();
}

template <Giffable T>
uint8_t giffable_factory<T>::get_type(const std::string &tag)
{
    return processor_->get_type(tag).value_or(0);
}

template <Giffable T>
uint64_t giffable_factory<T>::get_delay()
{
    return processor_->get_frame_delay();
}

}