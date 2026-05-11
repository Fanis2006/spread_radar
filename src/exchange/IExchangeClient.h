#pragma once

#include "core/Ticker.h"

#include <string>

class IExchangeClient {
public:
    virtual ~IExchangeClient() = default;

    virtual std::string name() const = 0;

    virtual Ticker getTicker(const std::string& symbol) = 0;
};