#include "core/SpreadCalculator.h"

#include <limits>

std::optional<SimulationResult> SpreadCalculator::simulate(
    const std::vector<Ticker>& tickers,
    double usdtAmount,
    const std::map<std::string, FeeConfig>& fees
) const {
    std::vector<Ticker> validTickers;

    for (const auto& ticker : tickers) {
        if (ticker.ok && ticker.bid > 0.0 && ticker.ask > 0.0) {
            validTickers.push_back(ticker);
        }
    }

    if (validTickers.size() < 2) {
        return std::nullopt;
    }

    bool foundPair = false;
    SimulationResult bestResult;
    double bestNetProfit = -std::numeric_limits<double>::infinity();

    for (const auto& buyTicker : validTickers) {
        for (const auto& sellTicker : validTickers) {
            if (buyTicker.exchange == sellTicker.exchange) {
                continue;
            }

            double buyFee = 0.001;
            double sellFee = 0.001;

            if (fees.count(buyTicker.exchange) > 0) {
                buyFee = fees.at(buyTicker.exchange).tradingFeePct;
            }

            if (fees.count(sellTicker.exchange) > 0) {
                sellFee = fees.at(sellTicker.exchange).tradingFeePct;
            }

            SimulationResult current;

            current.buyExchange = buyTicker.exchange;
            current.sellExchange = sellTicker.exchange;

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
        return std::nullopt;
    }

    return bestResult;
}