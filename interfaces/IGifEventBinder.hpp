#pragma once
#include <functional>

class IGifEventBinder
{
public:
    virtual void set_on_frame(std::function<void(int)>) = 0;
    virtual ~IGifEventBinder() = default;
};

