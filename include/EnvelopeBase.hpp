#pragma once

#include <limits>
#include <cmath>
#include <algorithm>

enum class EnvelopeStatus {
    OK,
    AboveUpperLimit,
    BelowLowerLimit,
    ViolationTimeout
};

enum class EnvelopeArrayStatus {
    OK,
    AnyAboveUpperLimit,
    AnyBelowLowerLimit,
    ViolationTimeout,
    NoReferenceSet,
    SizeMismatch
};

enum class EnvelopeMarginMode {
    Percent,
    Absolute
};

struct EnvelopeBaseConfig {
    EnvelopeMarginMode mode = EnvelopeMarginMode::Percent;
    double marginUpper = 0;
    double marginLower = 0;
    double clampMin = __DBL_MIN__;
    double clampMax = __DBL_MAX__;
    double minMargin = __DBL_MIN__;
    double violationTimeout = 0; // max time out of bounds
};

[[nodiscard]] inline double computeEnvelopeMargin(const EnvelopeBaseConfig& config, double ref, double margin) {
    // Calculated margin will be clamped to minMargin
    if (config.mode == EnvelopeMarginMode::Percent) {
        return std::max(std::fabs(ref * margin / 100.0), config.minMargin);
    } else {
        return std::fmax(margin, config.minMargin);
    }
}
