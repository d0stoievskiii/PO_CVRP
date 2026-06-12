#pragma once

#include "common.hpp"

Solution bestRelocate(const Solution& current, const CVRPInstance& instance);

Solution bestSwap(const Solution& current, const CVRPInstance& instance);

Solution bestTwoOptIntra(const Solution& current, const CVRPInstance& instance);

Solution VND(const Solution& initial, const CVRPInstance& instance, const VNDConfig& config);