#pragma once
#include <string>
#include <map>
#include <tuple>
#include <cstdint>
#include <filesystem>
#include <optional>


class IGiffable 
{
public:
virtual void load_image(const std::filesystem::path &file) = 0; //load file
virtual void process() = 0; //process gif source
virtual std::map<std::string,int> get_table() const = 0; //get table {tag name:frame quantity}
virtual std::tuple<uint8_t,std::size_t> get_next_frame(const std::string&, uint32_t) const = 0;//по названию передавать кадры (по одному)
virtual std::optional<uint8_t> get_type(const std::string&) = 0; // 0x01 - cycled, 0x02 - pingpong, 0x03 - cycled reversed, 0x04 pingpong reversed
virtual uint64_t get_frame_delay() = 0; //frame delay in ms
virtual ~IGiffable() = default;
};