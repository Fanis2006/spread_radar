#include "core/FeeConfig.h"
#include "core/SpreadCalculator.h"

#include "exchange/BinanceClient.h"
#include "exchange/CoinbaseClient.h"
#include "exchange/KrakenClient.h"

#include <windows.h>
#include <clocale>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    std::string symbol;

    std::cout << "Выберите актив:\n";
    std::cout << "1. BTC-USDT\n";
    std::cout << "2. ETH-USDT\n";
    std::cout << "3. SOL-USDT\n";
    std::cout << "Ваш выбор: ";

    int choice = 0;
    std::cin >> choice;

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
        std::cout << "Некорректный выбор актива.\n";
        return 1;
    }

    double simulationAmount = 1000.0;

    std::cout << "Введите сумму симуляции в USDT: ";
    std::cin >> simulationAmount;

    if (!std::cin || simulationAmount <= 0.0) {
        std::cout << "Invalid amount. Amount must be greater than zero.\n";
        return 1;
    }

    BinanceClient binance;
    CoinbaseClient coinbase;
    KrakenClient kraken;

    std::vector<Ticker> tickers;

    tickers.push_back(binance.getTicker(symbol));
    tickers.push_back(coinbase.getTicker(symbol));
    tickers.push_back(kraken.getTicker(symbol));

    std::map<std::string, FeeConfig> fees;

    fees["Binance"] = FeeConfig{ 0.0 };    // demo mode: no fees
    fees["Coinbase"] = FeeConfig{ 0.0 };   // demo mode: no fees
    fees["Kraken"] = FeeConfig{ 0.0 };     // demo mode: no fees

    SpreadCalculator calculator;
    auto result = calculator.simulate(tickers, simulationAmount, fees);

    std::cout << "Spread Radar\n\n";

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Актив: " << symbol << "\n\n";

    std::cout << std::left
        << std::setw(12) << "Биржа"
        << std::right
        << std::setw(15) << "Bid"
        << std::setw(15) << "Ask"
        << "\n";

    std::cout << std::string(42, '-') << "\n";

    for (const auto& ticker : tickers) {
        if (!ticker.ok) {
            std::cout << std::left
                << std::setw(12) << ticker.exchange
                << "ОШИБКА: " << ticker.error << "\n";
            continue;
        }

        std::cout << std::left
            << std::setw(12) << ticker.exchange
            << std::right
            << std::setw(15) << ticker.bid
            << std::setw(15) << ticker.ask
            << "\n";
    }

    std::cout << "\n";

    if (!result.has_value()) {
        std::cout << "Недостаточно данных для расчёта спреда.\n";
        return 1;
    }

    const SimulationResult& simulation = result.value();

    std::cout << "Лучший маршрут симуляции:\n";

    std::cout << "Купить на:  "
        << simulation.buyExchange
        << " по ask "
        << simulation.bestAsk
        << "\n";

    std::cout << "Продать на: "
        << simulation.sellExchange
        << " по bid "
        << simulation.bestBid
        << "\n\n";

    std::cout << "Спред:\n";

    std::cout << "Абсолютный спред: "
        << simulation.spreadAbs
        << " USDT\n";

    std::cout << "Спред в процентах: "
        << simulation.spreadPct
        << "%\n\n";

    std::cout << "Комиссии: не учитываются\n\n";

    std::cout << "Сумма симуляции: "
        << simulation.startUSDT
        << " USDT\n";

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Прибыль без комиссий: "
        << simulation.rawProfit
        << " USDT ("
        << simulation.rawProfitPct
        << "%)\n";

    std::cout << "Итоговая прибыль: "
        << simulation.netProfit
        << " USDT ("
        << simulation.netProfitPct
        << "%)\n\n";

    return 0;
}