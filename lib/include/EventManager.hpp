#pragma once
/*
 * Copyright (c) 2026 David McFarland
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <string_view>
#include <vector>

namespace LuxLibrary {
    class ProducerBase; // Forward declaration.

    template < typename T > using EventQueue  = std::queue< T >;
    template < typename T > using EventVector = std::vector< T >;

    enum class EventType : std::uint8_t {
        KeyboardInput,
        MouseButtonPress,
    };

    class NewEvent {
      private:
        EventType event_type{};

      public:
        explicit NewEvent( const EventType type = EventType::KeyboardInput ) noexcept
            : event_type{ type } {
        }

        constexpr auto type() const noexcept -> EventType {
            return event_type;
        }

        constexpr auto name() const noexcept -> std::string_view {
            switch ( event_type ) {
                case EventType::KeyboardInput   : return { "Keyboard" };
                case EventType::MouseButtonPress: return { "Mousebutton" };
            };
        }
    };

    class EventSystem {
      private:
        EventQueue< NewEvent > event_queue;
        EventVector< ProducerBase* > subscribers;

        mutable std::mutex _mutex{};
        std::condition_variable condition_variaible{};
        std::atomic< bool > exit_thread{ false };

        auto wait() noexcept -> void {
            std::unique_lock< std::mutex > lock( _mutex );
            condition_variaible.wait(
                lock, [this] { return exit() || !empty(); } ); // sleep if false.
        }

        auto empty() const noexcept -> bool {
            return event_queue.empty();
        }

        auto push( const NewEvent& message ) noexcept -> void {
            std::scoped_lock lock( _mutex );
            event_queue.push( message );
            condition_variaible.notify_one();
        }

        auto pop() noexcept -> NewEvent {
            std::unique_lock< std::mutex > lock( _mutex );
            condition_variaible.wait( lock, [this] { return !event_queue.empty(); } );
            NewEvent message = event_queue.front();
            event_queue.pop();
            return message;
        }

        constexpr auto exit() const noexcept -> bool {
            return exit_thread;
        }

      public:
        constexpr auto process_events() noexcept -> void;

        auto register_system( ProducerBase* system ) -> void {
            std::scoped_lock lock( _mutex );
            subscribers.push_back( system );
        }

        auto quit() noexcept -> void {
            std::scoped_lock lock( _mutex );
            exit_thread = true;
            condition_variaible.notify_one();
        }

        constexpr auto handle() noexcept -> EventSystem* {
            return this;
        }

        friend class EventDispatcher;
    };

    class EventDispatcher {
      private:
        EventSystem* event_system{};

      public:
        explicit EventDispatcher( EventSystem* event_system )
            : event_system{ event_system } {
        }
        constexpr auto new_event( const NewEvent& event ) noexcept -> void {
            event_system->push( event );
        }
    };

    class ProducerBase {
      private:
      public:
        ProducerBase()          = default;
        virtual ~ProducerBase() = default;

        ProducerBase( const ProducerBase& )            = default;
        ProducerBase( ProducerBase&& )                 = default;
        ProducerBase& operator=( const ProducerBase& ) = default;
        ProducerBase& operator=( ProducerBase&& )      = default;

        virtual constexpr auto type() const noexcept -> EventType = 0;
        virtual auto handle_event() noexcept -> void              = 0;

        virtual auto handle() noexcept -> ProducerBase* = 0;
    };

    class NewKeyboard : public ProducerBase {
      private:
        EventType _type_producer_handles{ EventType::KeyboardInput };

      public:
        NewKeyboard()                                = default;
        ~NewKeyboard() override                      = default;
        NewKeyboard( const NewKeyboard& )            = default;
        NewKeyboard( NewKeyboard&& )                 = default;
        NewKeyboard& operator=( const NewKeyboard& ) = default;
        NewKeyboard& operator=( NewKeyboard&& )      = default;

        constexpr auto type() const noexcept -> EventType override {
            return _type_producer_handles;
        };

        auto handle_event() noexcept -> void override {
		  fmt::println( "Keyboard event handled." );
        }

        auto handle() noexcept -> NewKeyboard* override {
            return this;
        }
    };

    constexpr auto EventSystem::process_events() noexcept -> void {
        while ( !exit_thread ) {
            wait();
            if ( !empty() ) {
                auto msg{ pop() };
                for ( auto& subscriber : subscribers ) {
                    if ( msg.type() == subscriber->type() ) { subscriber->handle_event(); }
                }
            }
            if ( exit() && empty() ) { break; }
        }
		fmt::println(std::cerr, "Exiting event thread." );
    }

}; // namespace LuxLibrary
