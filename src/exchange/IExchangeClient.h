#pragma once

#include "core/TokenPrice.h"

#include <string>

class IExchangeClient {
public:
    virtual ~IExchangeClient() = default;

    virtual std::string name() const = 0;

    virtual TokenPrice getTicker(const std::string& symbol) = 0;
};