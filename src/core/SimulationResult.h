#pragma once

#include <string>

struct SimulationResult {
    std::string buySite;
    std::string sellSite;

    double bestAsk = 0.0;
    double bestBid = 0.0;

    double spreadAbs = 0.0;
    double spreadPct = 0.0;

    double buyFeePct = 0.0;
    double sellFeePct = 0.0;

    double startUSDT = 0.0;
    double assetAmountBeforeFee = 0.0;
    double assetAmountAfterFee = 0.0;

    double rawProfit = 0.0;
    double rawProfitPct = 0.0;

    double netProfit = 0.0;
    double netProfitPct = 0.0;

    bool profitable = false;
};