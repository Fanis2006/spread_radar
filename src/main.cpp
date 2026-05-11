#include "core/Fee.h"
#include "core/SpreadCalculator.h"

#include "exchange/BinanceClient.h"
#include "exchange/CoinbaseClient.h"
#include "exchange/KrakenClient.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <windows.h>
#include <clocale>

using namespace std;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    string symbol;

    cout << "Выберите актив:\n";
    cout << "1. BTC-USDT\n";
    cout << "2. ETH-USDT\n";
    cout << "3. SOL-USDT\n";
    cout << "Ваш выбор: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        symbol = "BTC-USDT";
    }
    else if (choice == 2) {
        symbol = "ETH-USDT";
    }
    else if (choice == 3) {
        symbol = "SOL-USDT";
    }
    else {
        cout << "Некорректный выбор актива.\n";
        return 1;
    }

    double simulationAmount;

    cout << "Введите сумму симуляции в USDT: ";
    cin >> simulationAmount;

    if (!cin || simulationAmount <= 0.0) {
        cout << "Некорректная сумма. Сумма должна быть больше нуля.\n";
        return 1;
    }

    BinanceClient binance;
    CoinbaseClient coinbase;
    KrakenClient kraken;

    vector<TokenPrice> prices;

    prices.push_back(binance.getTokenPrice(symbol));
    prices.push_back(coinbase.getTokenPrice(symbol));
    prices.push_back(kraken.getTokenPrice(symbol));

    map<string, Fee> fees;

    Fee binanceFee;
    binanceFee.FeePct = 0.0;

    Fee coinbaseFee;
    coinbaseFee.FeePct = 0.0;

    Fee krakenFee;
    krakenFee.FeePct = 0.0;

    fees["Binance"] = binanceFee;
    fees["Coinbase"] = coinbaseFee;
    fees["Kraken"] = krakenFee;

    SpreadCalculator calculator;
    auto result = calculator.simulate(prices, simulationAmount, fees);

    cout << "Spread Radar\n\n";

    cout << fixed << setprecision(2);

    cout << "Актив: " << symbol << "\n\n";

    cout << left
        << setw(12) << "Биржа"
        << right
        << setw(15) << "Bid"
        << setw(15) << "Ask"
        << "\n";

    cout << string(42, '-') << "\n";

    for (const auto& TokenPrice : prices) {
        if (!TokenPrice.ok) {
            cout << left
                << setw(12) << TokenPrice.site
                << "ОШИБКА: " << TokenPrice.error << "\n";
            continue;
        }

        cout << left
            << setw(12) << TokenPrice.site
            << right
            << setw(15) << TokenPrice.bid
            << setw(15) << TokenPrice.ask
            << "\n";
    }

    cout << "\n";

    if (!result.has_value()) {
        cout << "Недостаточно данных для расчёта спреда.\n";
        return 1;
    }

    const SimulationResult& simulation = result.value();

    cout << "Лучший маршрут симуляции:\n";

    cout << "Купить на:  "
        << simulation.buySite
        << " по ask "
        << simulation.bestAsk
        << "\n";

    cout << "Продать на: "
        << simulation.sellSite
        << " по bid "
        << simulation.bestBid
        << "\n\n";

    cout << "Спред:\n";

    cout << "Абсолютный спред: "
        << simulation.spreadAbs
        << " USDT\n";

    cout << "Спред в процентах: "
        << simulation.spreadPct
        << "%\n\n";

    cout << "Комиссии: не учитываются\n\n";

    cout << "Сумма симуляции: "
        << simulation.startUSDT
        << " USDT\n";

    cout << "Расчётная прибыль: "
        << simulation.profit
        << " USDT ("
        << simulation.profitPct
        << "%)\n";

    return 0;
}