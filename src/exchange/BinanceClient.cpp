#include "exchange/BinanceClient.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <string>

using namespace std;
using json = nlohmann::json;

string BinanceClient::name() const {
    return "Binance";
}

TokenPrice BinanceClient::getTicker(const string& token) {
    TokenPrice price;
    price.site = name();
    price.token = token;

    try {
        string binanceSymbol;

        if (token == "BTC-USDT") {
            binanceSymbol = "BTCUSDT";
        }
        else if (token == "ETH-USDT") {
            binanceSymbol = "ETHUSDT";
        }
        else if (token == "SOL-USDT") {
            binanceSymbol = "SOLUSDT";
        }
        else {
            price.ok = false;
            price.error = "Unsupported token";
            return price;
        }

        const string url =
            "https://api.binance.com/api/v3/ticker/bookTicker?symbol=" + binanceSymbol;

        string response = httpClient.get(url);

        json data = json::parse(response);

        price.bid = stod(data.at("bidPrice").get<string>());
        price.ask = stod(data.at("askPrice").get<string>());
        price.ok = true;
    }
    catch (const exception& e) {
        price.ok = false;
        price.error = e.what();
    }

    return price;
}