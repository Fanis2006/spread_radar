#pragma once

#include <string>

struct SimulationResult {
    std::string buySite;
    std::string sellSite;

    double bestAsk;
    double bestBid;

    double spreadAbs;
    double spreadPct;

    double buyFeePct;
    double sellFeePct;

    double startUSDT;

    double profit;
    double profitPct;

    double realProfit;
    double realProfitPct;

    bool profitable = false;
};