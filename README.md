# GIF Animation Library

[🇷🇺 Русская версия](#rus) | [🇬🇧 English version](#eng)


# RUS

## Диграмма классов
```mermaid
classDiagram
    %% Main Interfaces
    class IGiffable {
        <<interface>>
        +load_image(file) void
        +process() void
        +get_table() map~string,int~
        +get_next_frame(tag, index) tuple~uint8_t,size_t~
        +get_type(tag) optional~uint8_t~
        +get_frame_delay() uint64_t
    }

    class IGifEventBinder {
        <<interface>>
        +set_on_frame(callback) void
    }

    %% Core Components
    class gif_animator {
        +update(current_time_ms) void
        +toggle() void
        +should_render() bool
        +get_image() tuple~uint8_t*,size_t~
        +set_on_frame(callback) void
        -handler_ : gif_node_handler
        -frame_delay_ms_ : uint64_t
        -last_update_called_ : uint64_t
        -playing_ : bool
    }
    IGifEventBinder <|.. gif_animator : implements

    class giffable_factory~T~ {
        +process_data(file) unordered_map~string, gif_animator~
        -processor_ : unique_ptr~IGiffable~
        -clear() void
        -reset() void
        -load_file(file) void
        -get_table() unordered_map~string,int~
        -get_type(tag) uint8_t
        -get_delay() uint64_t
    }
    giffable_factory ..> IGiffable : uses

    %% Internal Implementation
    class gif_node_handler {
        +create_cycle() void
        +create_pingpong() void
        +reverse() void
        +add_data(data, size) void
        +get_data() tuple~uint8_t*,size_t,size_t~
        -node_ : gif_node*
        -first_element_ : gif_node*
        -status_ : LIST_STATUS
    }

    class gif_node {
        -prev_ : gif_node*
        -next_ : gif_node*
        -size_ : size_t
        -data_ : uint8_t*
        +get_data() tuple~uint8_t*,size_t~
        +create_next(next, size) gif_node*
    }

    gif_animator *-- gif_node_handler : contains
    gif_node_handler *-- gif_node : manages
    giffable_factory ..> gif_animator : creates
```

## Обзор
Библиотека предоставляет систему для загрузки, обработки и воспроизведения анимированных GIF. Основные компоненты:

### 1. Интерфейсы
- **IGiffable.hpp**  
  Базовый интерфейс для работы с GIF:
  ```cpp
  virtual void load_image(const std::filesystem::path &file) = 0;
  virtual std::tuple<uint8_t,std::size_t> get_next_frame(...) = 0;
  virtual std::map<std::string,int> get_table() const = 0;
  ```

- **IGifEventBinder.hpp**  
  Интерфейс для callback-событий:
  ```cpp
  virtual void set_on_frame(std::function<void(int)>) = 0;
  ```

### 2. GIF Animator
**gif_animator.hpp/cpp**  
Контроллер воспроизведения анимации:
```cpp
class gif_animator : public IGifEventBinder {
public:
    void update(uint64_t current_time_ms); // Обновление состояния
    void toggle(); // Запуск/пауза
    std::tuple<uint8_t*,std::size_t> get_image() const; // Текущий кадр
};
```
Особенности:
- Управление временем кадров
- Callback при смене кадра
- Поддержка перемещения объектов

### 3. Фабрика обработки
**giffable_factory.hpp**  
Создает аниматоры из GIF-файлов:
```cpp
template <Giffable T>
std::unordered_map<std::string, gif_animator> 
process_data(const std::filesystem::path &file) {
    // Автоматически создает аниматоры для всех тегов
}
```
Особенности:
- Поддержка разных типов анимации (цикл, пинг-понг)
- Автоматическое определение параметров GIF
- Возвращает готовые к использованию аниматоры

## Внутренние компоненты (кратко)
- **gif_node** - Узел двусвязного списка для хранения кадров
- **gif_node_handler** - Менеджер последовательности кадров
- Реализует логику цикличности, пинг-понга и реверсивного воспроизведения

## Пример использования
```cpp
auto factory = giffable_factory<MyGifProcessor>();
auto animators = factory.process_data("animation.gif");

auto& animator = animators["main"];
animator.set_on_frame([](int frame){ 
    std::cout << "Frame: " << frame << std::endl;
});

// В игровом цикле:
animator.update(current_time);
if (animator.should_render()) {
    auto [frame, size] = animator.get_image();
    render(frame, size);
}
```

## Зависимости
- C++17 (filesystem, optional)
- STL контейнеры и алгоритмы
- Кроссплатформенная реализация

Библиотека инкапсулирует сложную логику работы с GIF, предоставляя простой API для интеграции анимации в приложения.

---
---
# ENG

## Class diagram

```mermaid
classDiagram
    %% Main Interfaces
    class IGiffable {
        <<interface>>
        +load_image(file) void
        +process() void
        +get_table() map~string,int~
        +get_next_frame(tag, index) tuple~uint8_t,size_t~
        +get_type(tag) optional~uint8_t~
        +get_frame_delay() uint64_t
    }

    class IGifEventBinder {
        <<interface>>
        +set_on_frame(callback) void
    }

    %% Core Components
    class gif_animator {
        +update(current_time_ms) void
        +toggle() void
        +should_render() bool
        +get_image() tuple~uint8_t*,size_t~
        +set_on_frame(callback) void
        -handler_ : gif_node_handler
        -frame_delay_ms_ : uint64_t
        -last_update_called_ : uint64_t
        -playing_ : bool
    }
    IGifEventBinder <|.. gif_animator : implements

    class giffable_factory~T~ {
        +process_data(file) unordered_map~string, gif_animator~
        -processor_ : unique_ptr~IGiffable~
        -clear() void
        -reset() void
        -load_file(file) void
        -get_table() unordered_map~string,int~
        -get_type(tag) uint8_t
        -get_delay() uint64_t
    }
    giffable_factory ..> IGiffable : uses

    %% Internal Implementation
    class gif_node_handler {
        +create_cycle() void
        +create_pingpong() void
        +reverse() void
        +add_data(data, size) void
        +get_data() tuple~uint8_t*,size_t,size_t~
        -node_ : gif_node*
        -first_element_ : gif_node*
        -status_ : LIST_STATUS
    }

    class gif_node {
        -prev_ : gif_node*
        -next_ : gif_node*
        -size_ : size_t
        -data_ : uint8_t*
        +get_data() tuple~uint8_t*,size_t~
        +create_next(next, size) gif_node*
    }

    gif_animator *-- gif_node_handler : contains
    gif_node_handler *-- gif_node : manages
    giffable_factory ..> gif_animator : creates
```

## Overview
A C++ library for loading, processing, and animating GIF images. Provides a high-level API for GIF manipulation while handling low-level details internally.

## Core Interfaces
- **IGiffable.hpp**  
  Base interface for GIF processing:
  ```cpp
  virtual void load_image(const std::filesystem::path &file) = 0;
  virtual std::tuple<uint8_t,std::size_t> get_next_frame(...) = 0;
  virtual std::map<std::string,int> get_table() const = 0;
  ```

- **IGifEventBinder.hpp**  
  Frame event callback interface:
  ```cpp
  virtual void set_on_frame(std::function<void(int)>) = 0;
  ```

## Animation Controller
**gif_animator.hpp/cpp**  
Manages GIF playback:
```cpp
class gif_animator : public IGifEventBinder {
public:
    void update(uint64_t current_time_ms); // Update animation state
    void toggle(); // Play/pause toggle
    std::tuple<uint8_t*,std::size_t> get_image() const; // Current frame
};
```
Features:
- Frame timing control
- Frame change callbacks
- Move semantics support

## Processing Factory
**giffable_factory.hpp**  
Creates ready-to-use animators from GIF files:
```cpp
template <Giffable T>
std::unordered_map<std::string, gif_animator> 
process_data(const std::filesystem::path &file) {
    // Automatically processes GIF and creates animators
}
```
Key capabilities:
- Supports multiple animation types (loop, ping-pong)
- Automatic GIF parameter detection
- Returns pre-configured animators

## Internal Components
- **gif_node**: Frame storage node (doubly-linked list)
- **gif_node_handler**: Manages frame sequences and playback modes
- Handles animation logic (cycling, ping-pong, reversing)

## Basic Usage
```cpp
// Create animators from GIF
auto factory = giffable_factory<CustomGifProcessor>();
auto animators = factory.process_data("animation.gif");

// Configure animation
auto& anim = animators["main"];
anim.set_on_frame([](int frame){ 
    std::cout << "Displaying frame: " << frame << std::endl;
});

// Animation loop
anim.update(current_time);
if (anim.should_render()) {
    auto [frame_data, size] = anim.get_image();
    display_frame(frame_data, size);
}
```

## Dependencies
- C++17 (filesystem, optional)
- STL containers
- Cross-platform

Provides simple GIF animation integration while handling complex decoding and timing internally.

