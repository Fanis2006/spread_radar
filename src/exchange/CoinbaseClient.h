#pragma once

#include "exchange/IExchangeClient.h"
#include "utils/HttpClient.h"

class CoinbaseClient : public IExchangeClient {
public:
    std::string name() const override;

    Ticker getTicker(const std::string& symbol) override;

private:
    HttpClient httpClient;
};