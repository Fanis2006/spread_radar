#include "exchange/CoinbaseClient.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <string>

using json = nlohmann::json;

std::string CoinbaseClient::name() const {
    return "Coinbase";
}

Ticker CoinbaseClient::getTicker(const std::string& symbol) {
    Ticker ticker;
    ticker.exchange = name();
    ticker.symbol = symbol;

    try {
        std::string coinbaseSymbol = symbol;

        if (symbol == "BTC-USDT") {
            coinbaseSymbol = "BTC-USDT";
        }

        const std::string url =
            "https://api.exchange.coinbase.com/products/" + coinbaseSymbol + "/ticker";

        std::string response = httpClient.get(url);

        json data = json::parse(response);

        ticker.bid = std::stod(data.at("bid").get<std::string>());
        ticker.ask = std::stod(data.at("ask").get<std::string>());
        ticker.last = std::stod(data.at("price").get<std::string>());
        ticker.ok = true;
    }
    catch (const std::exception& e) {
        ticker.ok = false;
        ticker.error = e.what();
    }

    return ticker;
}