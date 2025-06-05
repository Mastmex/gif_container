#include "gif_node.hpp"
#include "gif_node_handler.hpp"
#include "gif_animator.hpp"
#include <gtest/gtest.h>

using namespace mm_gif;

TEST(gif_node_handler, base_cycle_test)
{
    gif_node_handler hndl;
    unsigned char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);

    hndl.create_cycle();
    bool all_non_null = true;
    for (int i = 0; i < 20; i++) {
        auto [data, size, frame] = hndl.get_data();

        all_non_null &= (data != nullptr && size == 1 && frame >= 1 && frame <= 9 );
    }
    ASSERT_TRUE(all_non_null);
}

TEST(gif_node_handler, base_pingpong_test)
{
    gif_node_handler hndl;
    unsigned char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);

    hndl.create_pingpong();
    bool all_non_null = true;
    for (int i = 0; i < 20; i++) {
        auto [data, size, frame] = hndl.get_data();

        all_non_null &= (data != nullptr && size == 1 && frame >= 1 && frame <= 9 );
    }
    ASSERT_TRUE(all_non_null);
}

TEST(gif_node_handler, destructor_cleans_up)
{
    bool success = true;
    {
        gif_node_handler* hndl = new gif_node_handler;
        char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
        for (int i = 0; i < 9; i++)
            hndl->add_data(&arr[i], 1);
        hndl->create_cycle();
        for (int i = 0; i < 20; i++) {
            auto [data, size, frame] = hndl->get_data();
            success &= (data != nullptr && size == 1 && frame >= 1 && frame <= 9 );
        }
        delete hndl;
    }
    ASSERT_TRUE(success);
}

TEST(gif_node_handler, move_assignment_preserves_state)
{
    gif_node_handler hndl;
    char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_cycle();

    gif_node_handler hndl2;
    hndl2 = std::move(hndl);
    bool valid_data = true;
    for (int i = 0; i < 20; i++) {
        auto [data, size, frame] = hndl2.get_data();
        valid_data &= (data != nullptr && size == 1 && frame >= 1 && frame <= 9 );
    }
    ASSERT_TRUE(valid_data);
}

TEST(gif_node_handler, move_constructor_preserves_state)
{
    gif_node_handler hndl;
    unsigned char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_cycle();

    gif_node_handler hndl2(std::move(hndl));
    bool valid_data = true;
    for (int i = 0; i < 20; i++) {
        auto [data, size, frame] = hndl2.get_data();

        valid_data &= (data != nullptr && size == 1 && frame >= 1 && frame <= 9 );
    }
    ASSERT_TRUE(valid_data);
}

TEST(gif_node_handler, empty_handler_returns_null)
{
    gif_node_handler hndl;
    auto [data, size, frame] = hndl.get_data();
    ASSERT_EQ(data, nullptr);
    ASSERT_EQ(size, 0);
    ASSERT_EQ(frame, 0);
}

TEST(gif_node_handler, reverse_direction_switching)
{
    gif_node_handler hndl;
    unsigned char arr[3] = {'A', 'B', 'C'};
    for (int i = 0; i < 3; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_pingpong();

    hndl.reverse(); // switch to REVERSED
    auto [data1, size1, frame1] = hndl.get_data();
    hndl.reverse(); // switch back to FORWARD
    auto [data2, size2, frame2] = hndl.get_data();

    ASSERT_NE(data1, nullptr);
    ASSERT_NE(data2, nullptr);
    ASSERT_NE(frame1, frame2); // should yield different frames due to direction switch
}

TEST(gif_node_handler, set_default_resets_state)
{
    gif_node_handler hndl;
    unsigned char arr[4] = {'1','2','3','4'};
    for (int i = 0; i < 4; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_cycle();
    for (int i = 0; i < 3; i++)
        hndl.get_data();

    hndl.set_default();
    auto [data, size, frame] = hndl.get_data();
    ASSERT_EQ(frame, 1); // Should reset to the first frame
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(size, 1);
}

TEST(gif_animator, default_ctor)
{
    gif_node_handler hndl;
    unsigned char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_cycle();
    gif_animator anim(std::move(hndl),100);
    anim.toggle();
    // uint64_t time = 0;
    // for(int i=0;i<100;++i)
    // {
    //     anim.update(time);
    //     if(anim.should_render())
    //     {
    //         auto [frame, size] = anim.get_image();
    //         std::cout<<"Current frame="<<*frame<<"\tCurrent size="<<size<<"\n";
    //     }
    //     std::cout<<"Current iteration="<<i<<"\tCurrent time="<<time<<"\n";
    //     std::cout<<"---------------------------------------------------\n";
    //     time += 1000/60;
    // }
}

TEST(gif_animator, move_ctor)
{
    gif_node_handler hndl;
    unsigned char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_cycle();
    gif_animator anim1(std::move(hndl),100);
    auto anim{std::move(anim1)};
    anim.toggle();
    // uint64_t time = 0;
    // for(int i=0;i<100;++i)
    // {
    //     anim.update(time);
    //     if(anim.should_render())
    //     {
    //         auto [frame, size] = anim.get_image();
    //         std::cout<<"Current frame="<<*frame<<"\tCurrent size="<<size<<"\n";
    //     }
    //     std::cout<<"Current iteration="<<i<<"\tCurrent time="<<time<<"\n";
    //     std::cout<<"---------------------------------------------------\n";
    //     time += 1000/60;
    // }
}

TEST(gif_animator, move_assignment)
{
    gif_node_handler hndl;
    unsigned char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_cycle();
    gif_animator anim1(std::move(hndl),100);
    auto anim = std::move(anim1);
    anim.toggle();
    // uint64_t time = 0;
    // for(int i=0;i<100;++i)
    // {
    //     anim.update(time);
    //     if(anim.should_render())
    //     {
    //         auto [frame, size] = anim.get_image();
    //         std::cout<<"Current frame="<<*frame<<"\tCurrent size="<<size<<"\n";
    //     }
    //     std::cout<<"Current iteration="<<i<<"\tCurrent time="<<time<<"\n";
    //     std::cout<<"---------------------------------------------------\n";
    //     time += 1000/60;
    // }
}

TEST(gif_animator, callback)
{
    gif_node_handler hndl;
    unsigned char arr[10] = {'1','2','3','4','5','6','7','8','9','0'};
    for (int i = 0; i < 9; i++)
        hndl.add_data(&arr[i], 1);
    hndl.create_cycle();
    gif_animator anim(std::move(hndl),100);
    anim.toggle();
    anim.set_on_frame([](int frame){ if(frame==3) std::cout<<"\n\n\t\tHELLO FROM CALLBACK\n\n";});
    uint64_t time = 0;
    for(int i=0;i<100;++i)
    {
        anim.update(time);
        if(anim.should_render())
        {
            auto [frame, size] = anim.get_image();
            std::cout<<"Current frame="<<*frame<<"\tCurrent size="<<size<<"\n";
        }
        std::cout<<"Current iteration="<<i<<"\tCurrent time="<<time<<"\n";
        std::cout<<"---------------------------------------------------\n";
        time += 1000/60;
    }
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
