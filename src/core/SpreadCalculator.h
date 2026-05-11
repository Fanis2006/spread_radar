#pragma once

#include "core/FeeConfig.h"
#include "core/SimulationResult.h"
#include "core/Ticker.h"

#include <map>
#include <optional>
#include <string>
#include <vector>

class SpreadCalculator {
public:
    std::optional<SimulationResult> simulate(
        const std::vector<Ticker>& tickers,
        double usdtAmount,
        const std::map<std::string, FeeConfig>& fees
    ) const;
};