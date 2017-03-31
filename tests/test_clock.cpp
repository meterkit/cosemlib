
extern "C" {
#include "clock.h"
}
#include "catch.hpp"

// This is our reference library to test with
#include "date.h"
#include <chrono>
#include <iostream>

using namespace date;
using namespace std::chrono;

// Quick simple tests to checks that the clock is working in nominal way
// Not full test
void SanityCheck()
{
    constexpr auto x1 = 2015_y/mar/sun[last];
    constexpr auto x4 = year_month_day{x1};

    int year  = x4.year().operator int();
    unsigned int month  = x4.month().operator unsigned int();
    unsigned int day  = x4.day().operator unsigned int();

    // Check day number
    uint32_t last_sun = clk_last_dow(year, month, SUNDAY);

    REQUIRE(last_sun == day);

    // Check DOW
    uint32_t dow = clk_dow(year, month, day);

    // Check that dow is sunday
    REQUIRE(dow == SUNDAY);

}


TEST_CASE( "Clock", "[clock_test]" )
{
    SanityCheck();

}



