/*
 * clock — time of day from the companion's updates plus esp_timer.
 * See clock.h.
 */

#include "esp_timer.h"

#include "clock.h"

#define SECONDS_PER_DAY (24 * 60 * 60)

static bool    s_valid;
static int32_t s_base_seconds;   /* time of day at the last update */
static int64_t s_base_us;        /* esp_timer value at the last update */

void clock_set(int hour, int minute, int second)
{
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        second < 0 || second > 59) {
        return;
    }
    s_base_seconds = hour * 3600 + minute * 60 + second;
    s_base_us      = esp_timer_get_time();
    s_valid        = true;
}

bool clock_valid(void) { return s_valid; }

static int32_t seconds_of_day(void)
{
    const int64_t elapsed = (esp_timer_get_time() - s_base_us) / 1000000;
    return (int32_t)((s_base_seconds + elapsed) % SECONDS_PER_DAY);
}

int32_t clock_hour_hand(void)
{
    const int32_t t = seconds_of_day();
    return ((t / 3600) % 12) * 300 + ((t / 60) % 60) * 5;
}

int32_t clock_minute_hand(void)
{
    const int32_t t = seconds_of_day();
    return ((t / 60) % 60) * 60 + (t % 60);
}

int32_t clock_second_hand(void)
{
    return (seconds_of_day() % 60) * 60;
}
