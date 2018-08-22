//
// Created by fatih on 8/22/18.
//

#pragma once

#include <tos/compiler.hpp>
#include <drivers/common/inet/tcp_stream.hpp>
#include <tos/expected.hpp>

struct net_facade
{
    tos::tcp_stream<tos::esp82::tcp_endpoint>& str;

    int ALWAYS_INLINE read(unsigned char* buffer, int len, int)
    {
        return with(str.read(tos::span<char>{ (char*)buffer, len }), [](auto& rd) {
            return rd.size();
        }, [](auto& err){
            return 0;
        });
    }

    int ALWAYS_INLINE write(unsigned char* buffer, int len, int)
    {
        str.write({ (char*)buffer, len });
        return len;
    }
};

struct timer_facade
{
public:
    timer_facade()
    {
        interval_end_ms = 0L;
    }

    timer_facade(int ms)
    {
        countdown_ms(ms);
    }

    bool ALWAYS_INLINE expired()
    {
        tos::this_thread::yield();
        return (interval_end_ms > 0L) && (millis() >= interval_end_ms);
    }

    void ALWAYS_INLINE countdown_ms(unsigned long ms)
    {
        interval_end_ms = millis() + ms;
    }

    void ALWAYS_INLINE countdown(int seconds)
    {
        countdown_ms((unsigned long)seconds * 1000L);
    }

    int ALWAYS_INLINE left_ms()
    {
        return interval_end_ms - millis();
    }

private:

    unsigned long ALWAYS_INLINE millis()
    {
        return system_get_time() / 1000;
    }

    unsigned long interval_end_ms;
};
