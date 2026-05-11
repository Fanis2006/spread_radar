#pragma once

#include "exchange/IExchangeClient.h"
#include "utils/HttpClient.h"

#include <string>

class KrakenClient : public IExchangeClient {
public:
    std::string name() const override;

    TokenPrice getTicker(const std::string& symbol) override;

private:
    HttpClient httpClient;
};