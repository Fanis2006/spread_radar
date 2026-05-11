#include "core/SpreadCalculator.h"

#include <limits>

using namespace std;

optional<SimulationResult> SpreadCalculator::simulate(
    const vector<TokenPrice>& prices,
    double usdtAmount,
    const map<string, Fee>& fees
) const {
    vector<TokenPrice> validPrices;

    for (const auto& price : prices) {
        if (price.ok && price.bid > 0.0 && price.ask > 0.0) {
            validPrices.push_back(price);
        }
    }

    if (validPrices.size() < 2) {
        return nullopt;
    }

    bool foundPair = false;
    SimulationResult bestResult;
    double bestrealProfit = -numeric_limits<double>::infinity();

    for (const auto& buyPrice : validPrices) {
        for (const auto& sellPrice : validPrices) {
            if (buyPrice.site == sellPrice.site) {
                continue;
            }

            double buyFee = 0.0;
            double sellFee = 0.0;

            if (fees.count(buyPrice.site) > 0) {
                buyFee = fees.at(buyPrice.site).FeePct;
            }

            if (fees.count(sellPrice.site) > 0) {
                sellFee = fees.at(sellPrice.site).FeePct;
            }

            SimulationResult current;

            current.buySite = buyPrice.site;
            current.sellSite = sellPrice.site;

            current.bestAsk = buyPrice.ask;
            current.bestBid = sellPrice.bid;

            current.spreadAbs = current.bestBid - current.bestAsk;
            current.spreadPct = current.spreadAbs / current.bestAsk * 100.0;

            current.buyFeePct = buyFee;
            current.sellFeePct = sellFee;

            current.startUSDT = usdtAmount;

            double coinAmount = usdtAmount / current.bestAsk;
            double coinAmountAfterFee = coinAmount * (1.0 - buyFee);

            double usdtAfterRawSell = coinAmount * current.bestBid;
            double usdtAfterNetSell = coinAmountAfterFee * current.bestBid * (1.0 - sellFee);

            current.profit = usdtAfterRawSell - usdtAmount;
            current.profitPct = current.profit / usdtAmount * 100.0;

            current.realProfit = usdtAfterNetSell - usdtAmount;
            current.realProfitPct = current.realProfit / usdtAmount * 100.0;

            if (current.realProfit > 0.0) {
                current.profitable = true;
            }
            else {
                current.profitable = false;
            }

            if (!foundPair || current.realProfit > bestrealProfit) {
                foundPair = true;
                bestrealProfit = current.realProfit;
                bestResult = current;
            }
        }
    }

    if (!foundPair) {
        return nullopt;
    }

    return bestResult;
}