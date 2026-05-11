#pragma once

#include "core/Fee.h"
#include "core/SimulationResult.h"
#include "core/TokenPrice.h"

#include <map>
#include <optional>
#include <string>
#include <vector>

class SpreadCalculator {
public:
    std::optional<SimulationResult> simulate(
        const std::vector<TokenPrice>& prices,
        double usdtAmount,
        const std::map<std::string, Fee>& fees
    ) const;
};