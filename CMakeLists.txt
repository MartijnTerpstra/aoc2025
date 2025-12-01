cmake_minimum_required(VERSION 3.13)

project(aoc)

macro(add_advent DAY)
    add_executable("day_${DAY}")
    target_sources("day_${DAY}"
        PRIVATE "day${DAY}/main.cpp"
        PRIVATE "day${DAY}/test_data.hpp"
        PRIVATE "day${DAY}/live_data.hpp")
    target_compile_features("day_${DAY}"
        PUBLIC cxx_std_23)
    target_compile_options("day_${DAY}"
        PUBLIC -fconstexpr-steps=111271242)
endmacro(add_advent)

add_advent(01)
