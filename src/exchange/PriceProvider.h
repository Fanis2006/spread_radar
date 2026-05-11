#pragma once

#include "core/TokenPrice.h"

#include <string>

class PriceProvider {
public:
    virtual std::string name() const = 0;

    virtual TokenPrice getTokenPrice(const std::string& token) = 0;
};