#pragma once

#include "exchange/PriceProvider.h"
#include "utils/HttpClient.h"

#include <string>

class KrakenClient : public PriceProvider {
public:
    std::string name() const override;

    TokenPrice getTokenPrice(const std::string& symbol) override;

private:
    HttpClient httpClient;
};