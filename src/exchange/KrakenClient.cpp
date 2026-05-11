#include "exchange/KrakenClient.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <string>

using namespace std;
using json = nlohmann::json;

string KrakenClient::name() const {
    return "Kraken";
}

TokenPrice KrakenClient::getTicker(const string& token) {
    TokenPrice price;
    price.site = name();
    price.token = token;

    try {
        string krakenSymbol;

        if (token == "BTC-USDT") {
            krakenSymbol = "XBTUSDT";
        }
        else if (token == "ETH-USDT") {
            krakenSymbol = "ETHUSDT";
        }
        else if (token == "SOL-USDT") {
            krakenSymbol = "SOLUSDT";
        }
        else {
            price.ok = false;
            price.error = "Unsupported token";
            return price;
        }

        const string url =
            "https://api.kraken.com/0/public/Ticker?pair=" + krakenSymbol;

        string response = httpClient.get(url);

        json data = json::parse(response);

        if (!data.at("error").empty()) {
            price.ok = false;
            price.error = data.at("error").dump();
            return price;
        }

        const auto& result = data.at("result");

        if (result.empty()) {
            price.ok = false;
            price.error = "Kraken returned empty result";
            return price;
        }

        const auto& firstPair = result.begin().value();

        price.ask = stod(firstPair.at("a").at(0).get<string>());
        price.bid = stod(firstPair.at("b").at(0).get<string>());
        price.ok = true;
    }
    catch (const exception& e) {
        price.ok = false;
        price.error = e.what();
    }

    return price;
}