#pragma once

#include <string>

struct TokenPrice {
    std::string site;
    std::string token;

    double bid;
    double ask;

    bool ok = false;
    std::string error;
};