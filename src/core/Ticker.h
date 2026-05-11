#pragma once

#include <string>

struct Ticker {
    std::string exchange;
    std::string symbol;

    double bid = 0.0;
    double ask = 0.0;
    double last = 0.0;

    bool ok = false;
    std::string error;
};