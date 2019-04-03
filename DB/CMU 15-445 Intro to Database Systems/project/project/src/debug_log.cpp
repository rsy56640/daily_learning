#include "include/debug_log.h"
#include "include/buffer_pool.h"
#include <iostream>
#include <ostream>
#include <iterator>
#include <algorithm>
using std::printf;
using std::cout;
using std::endl;

namespace DB::debug
{

    void ERROR_LOG(const char* msg)
    {
        std::cout << "Error Message: " << msg << std::endl;
    }

    void DEBUG_LOG(const char* msg)
    {
        std::cout << "Debug Message: " << msg << std::endl;
    }





    void debug_page(const page::page_id_t page_id, buffer::BufferPoolManager* buffer_pool)
    {
        printf("---------------------------------------------------\n");
        printf("page_id = %d\n", page_id);
        using namespace page;
        page::Page* node = buffer_pool->FetchPage(page_id);
        printf("nEntry = %d\n", node->get_nEntry());
        switch (node->get_page_t())
        {
        case page_t_t::INTERNAL:
            debug_internal(static_cast<const InternalPage*>(node));
            break;
        case page_t_t::LEAF:
            debug_leaf(static_cast<const LeafPage*>(node));
            break;
        case page_t_t::VALUE:
            debug_value(static_cast<const ValuePage*>(node));
            break;
        case page_t_t::ROOT_INTERNAL:
        case page_t_t::ROOT_LEAF:
            debug_root(static_cast<const RootPage*>(node));
            break;
        }
        node->unref();
        printf("---------------------------------------------------\n");
    }

    void debug_leaf(const page::LeafPage* leaf)
    {
        printf("LeafPage\n");
        printf("keys: ");
        std::copy(leaf->keys_, leaf->keys_ + leaf->get_nEntry(),
            std::ostream_iterator<int32_t>(std::cout, " "));
        printf("\n");
        printf("left = %d, right = %d\n", leaf->get_left_leaf(), leaf->get_right_leaf());
    }

    void debug_internal(const page::InternalPage* link)
    {
        printf("InternalPage\n");
        printf("keys: ");
        std::copy(link->keys_, link->keys_ + link->get_nEntry(),
            std::ostream_iterator<int32_t>(std::cout, " "));
        printf("\n");
        printf("branch: ");
        std::copy(link->branch_, link->branch_ + link->get_nEntry() + 1,
            std::ostream_iterator<int32_t>(std::cout, " "));
        printf("\n");
    }

    void debug_value(const page::ValuePage*)
    {
        printf("ValuePage\n");
    }

    void debug_root(const page::RootPage* root)
    {
        if (root->get_page_t() == page::page_t_t::ROOT_INTERNAL)
        {
            printf("ROOT_INTERNAL\n");
            printf("keys: ");
            std::copy(root->keys_, root->keys_ + root->get_nEntry(),
                std::ostream_iterator<int32_t>(std::cout, " "));
            printf("\n");
            printf("branch: ");
            std::copy(root->branch_, root->branch_ + root->get_nEntry() + 1,
                std::ostream_iterator<int32_t>(std::cout, " "));
            printf("\n");
        }
        else
        {
            printf("ROOT_LEAF\n");
            printf("keys: ");
            std::copy(root->keys_, root->keys_ + root->get_nEntry(),
                std::ostream_iterator<int32_t>(std::cout, " "));
            printf("\n");
        }
    }

} // end namespace DB::debug