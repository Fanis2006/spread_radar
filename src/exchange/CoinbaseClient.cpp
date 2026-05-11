#include "exchange/CoinbaseClient.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <string>

using namespace std;
using json = nlohmann::json;

string CoinbaseClient::name() const {
    return "Coinbase";
}

TokenPrice CoinbaseClient::getTicker(const string& token) {
    TokenPrice price;
    price.site = name();
    price.token = token;

    try {
        string coinbaseSymbol;

        if (token == "BTC-USDT") {
            coinbaseSymbol = "BTC-USDT";
        }
        else if (token == "ETH-USDT") {
            coinbaseSymbol = "ETH-USDT";
        }
        else if (token == "SOL-USDT") {
            coinbaseSymbol = "SOL-USDT";
        }
        else {
            price.ok = false;
            price.error = "Unsupported token";
            return price;
        }

        const string url =
            "https://api.exchange.coinbase.com/products/" + coinbaseSymbol + "/ticker";

        string response = httpClient.get(url);

        json data = json::parse(response);

        price.bid = stod(data.at("bid").get<string>());
        price.ask = stod(data.at("ask").get<string>());
        price.ok = true;
    }
    catch (const exception& e) {
        price.ok = false;
        price.error = e.what();
    }

    return price;
}