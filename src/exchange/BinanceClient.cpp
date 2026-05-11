#include "exchange/BinanceClient.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <string>

using json = nlohmann::json;

std::string BinanceClient::name() const {
    return "Binance";
}

Ticker BinanceClient::getTicker(const std::string& symbol) {
    Ticker ticker;
    ticker.exchange = name();
    ticker.symbol = symbol;

    try {
        std::string binanceSymbol = symbol;

        if (symbol == "BTC-USDT") {
            binanceSymbol = "BTCUSDT";
        }
        else if (symbol == "ETH-USDT") {
            binanceSymbol = "ETHUSDT";
        }
        else if (symbol == "SOL-USDT") {
            binanceSymbol = "SOLUSDT";
        }

        const std::string url =
            "https://api.binance.com/api/v3/ticker/bookTicker?symbol=" + binanceSymbol;

        std::string response = httpClient.get(url);

        json data = json::parse(response);

        ticker.bid = std::stod(data.at("bidPrice").get<std::string>());
        ticker.ask = std::stod(data.at("askPrice").get<std::string>());
        ticker.last = 0.0;
        ticker.ok = true;
    }
    catch (const std::exception& e) {
        ticker.ok = false;
        ticker.error = e.what();
    }

    return ticker;
}