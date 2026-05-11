#include "exchange/KrakenClient.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <string>

using json = nlohmann::json;

std::string KrakenClient::name() const {
    return "Kraken";
}

Ticker KrakenClient::getTicker(const std::string& symbol) {
    Ticker ticker;
    ticker.exchange = name();
    ticker.symbol = symbol;

    try {
        std::string krakenSymbol = symbol;

        if (symbol == "BTC-USDT") {
            krakenSymbol = "XBTUSDT";
        }
        else if (symbol == "ETH-USDT") {
            krakenSymbol = "ETHUSDT";
        }
        else if (symbol == "SOL-USDT") {
            krakenSymbol = "SOLUSDT";
        }

        const std::string url =
            "https://api.kraken.com/0/public/Ticker?pair=" + krakenSymbol;

        std::string response = httpClient.get(url);

        json data = json::parse(response);

        if (!data.at("error").empty()) {
            ticker.ok = false;
            ticker.error = data.at("error").dump();
            return ticker;
        }

        const auto& result = data.at("result");

        if (result.empty()) {
            ticker.ok = false;
            ticker.error = "Kraken returned empty result";
            return ticker;
        }

        const auto& firstPair = result.begin().value();

        ticker.ask = std::stod(firstPair.at("a").at(0).get<std::string>());
        ticker.bid = std::stod(firstPair.at("b").at(0).get<std::string>());
        ticker.last = std::stod(firstPair.at("c").at(0).get<std::string>());
        ticker.ok = true;
    }
    catch (const std::exception& e) {
        ticker.ok = false;
        ticker.error = e.what();
    }

    return ticker;
}