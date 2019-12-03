#include "../src/navigator.h"

#include <iostream>
#include <algorithm>
#include <array>

#define TEST_CASE(LABEL) test_case_t{LABEL, #LABEL}

//#define ITERABLE_EQUAL(LEFT, RIGHT) std::equal((LEFT).begin(), (LEFT).end(), (RIGHT).begin())

namespace {

using namespace subsonic_ipt;

constexpr double POINT_TOLERANCE{1e-8};

/// Trivial structure representing a labeled test case.
struct test_case_t {
    bool (* body)();

    std::string_view label;
};

bool test_navigator_directions()
{
    Navigator nav{};

    nav.apply_displacement(Point{10, 0});

    Point direction = nav.compute_direction();

    if (direction.dist_to(Point{-10, 0}) > POINT_TOLERANCE) {
        return false;
    }
    return true;
}

/// All test cases that will be run.
constexpr auto TEST_CASES = std::array{
    TEST_CASE(test_navigator_directions),
};

} // namespace

int main()
{
    bool pass_flag = true;

    const auto run_test = [&pass_flag](const test_case_t& test) {
        std::cout << "Running " << test.label << " ... ";
        if (!test.body()) {
            std::cout << "FAILED\n";
            pass_flag = false;
        } else {
            std::cout << "OK\n";
        }
    };

    std::for_each(TEST_CASES.begin(), TEST_CASES.end(), run_test);

    if (!pass_flag) {
        return 1;
    }
    return 0;
}
