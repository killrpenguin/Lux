#include "GarbageCollector.hpp"
#include "Common.hpp"

namespace LuxLibrary {

    GarbageCollector::~GarbageCollector() noexcept {
        while ( head ) {
            // Clean up iteratively to avoid stack overlfow via recursive destruction.
            uPtr next_node{ std::move( head->next ) };

            head = std::move( next_node );
        }
    }

    auto GarbageCollector::empty() const noexcept -> bool {
        return head == nullptr;
    }

    auto GarbageCollector::length() const noexcept -> size_type {
        return count;
    }

    auto GarbageCollector::push_back( const Value& val ) noexcept -> void {
        auto new_node{ std::make_unique< Node >( val ) };
        count++;

        if ( head == nullptr ) {
            head = std::move( new_node );
            return;
        }

        Node* current{ head.get() };

        while ( current->next != nullptr ) {
            current = current->next.get();
        }
        current->next = std::move( new_node );
    }

    auto GarbageCollector::push_front( const Value& val ) noexcept -> void {
        auto new_node{ std::make_unique< Node >( val ) };

        new_node->next = std::move( head );

        head = std::move( new_node );

        count++;
    }

    auto GarbageCollector::operator[]( const size_type index ) CLANG_LIFETIME_BOUND -> pointer {
        size_type cnt{};

        Node* current{ head.get() };

        while ( current != nullptr ) {
            if ( cnt == index ) { return &current->value; }

            current = current->next.get();

            cnt++;
        }
        return nullptr;
    }
}; // namespace LuxLibrary
