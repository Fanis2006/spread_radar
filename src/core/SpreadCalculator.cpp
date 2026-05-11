#include "core/SpreadCalculator.h"

#include <limits>

using namespace std;

optional<SimulationResult> SpreadCalculator::simulate(
    const vector<TokenPrice>& prices,
    double usdtAmount,
    const map<string, FeeConfig>& fees
) const {
    vector<TokenPrice> validTickers;

    for (const auto& TokenPrice : prices) {
        if (TokenPrice.ok && TokenPrice.bid > 0.0 && TokenPrice.ask > 0.0) {
            validTickers.push_back(TokenPrice);
        }
    }

    if (validTickers.size() < 2) {
        return nullopt;
    }

    bool foundPair = false;
    SimulationResult bestResult;
    double bestNetProfit = -numeric_limits<double>::infinity();

    for (const auto& buyTicker : validTickers) {
        for (const auto& sellTicker : validTickers) {
            if (buyTicker.site == sellTicker.site) {
                continue;
            }

            double buyFee = 0.001;
            double sellFee = 0.001;

            if (fees.count(buyTicker.site) > 0) {
                buyFee = fees.at(buyTicker.site).tradingFeePct;
            }

            if (fees.count(sellTicker.site) > 0) {
                sellFee = fees.at(sellTicker.site).tradingFeePct;
            }

            SimulationResult current;

            current.buySite = buyTicker.site;
            current.sellSite = sellTicker.site;

            current.bestAsk = buyTicker.ask;
            current.bestBid = sellTicker.bid;

            current.spreadAbs = current.bestBid - current.bestAsk;
            current.spreadPct = current.spreadAbs / current.bestAsk * 100.0;

            current.buyFeePct = buyFee;
            current.sellFeePct = sellFee;

            current.startUSDT = usdtAmount;

            current.assetAmountBeforeFee = usdtAmount / current.bestAsk;
            current.assetAmountAfterFee = current.assetAmountBeforeFee * (1.0 - buyFee);

            double usdtAfterRawSell = current.assetAmountBeforeFee * current.bestBid;
            double usdtAfterNetSell = current.assetAmountAfterFee * current.bestBid * (1.0 - sellFee);

            current.rawProfit = usdtAfterRawSell - usdtAmount;
            current.rawProfitPct = current.rawProfit / usdtAmount * 100.0;

            current.netProfit = usdtAfterNetSell - usdtAmount;
            current.netProfitPct = current.netProfit / usdtAmount * 100.0;

            current.profitable = current.netProfit > 0.0;

            if (!foundPair || current.netProfit > bestNetProfit) {
                foundPair = true;
                bestNetProfit = current.netProfit;
                bestResult = current;
            }
        }
    }

    if (!foundPair) {
        return nullopt;
    }

    return bestResult;
}