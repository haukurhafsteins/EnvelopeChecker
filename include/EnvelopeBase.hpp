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
    ViolationTimeout
};

enum class EnvelopeMarginMode {
    Absolute,
    Percent
};

template<typename T>
struct EnvelopeBaseConfig {
    EnvelopeMarginMode mode = EnvelopeMarginMode::Absolute;
    T marginUpper = T(0);
    T marginLower = T(0);
    T clampMin = std::numeric_limits<T>::lowest();
    T clampMax = std::numeric_limits<T>::max();
    T minMargin = T(0);
    T violationTimeout = T(0); // max time out of bounds
};

template<typename T>
[[nodiscard]] inline T computeEnvelopeMargin(const EnvelopeBaseConfig<T>& config, T ref, T margin) {
    if (config.mode == EnvelopeMarginMode::Percent) {
        return std::max(std::abs(ref) * margin / T(100), config.minMargin);
    } else {
        return std::max(margin, config.minMargin);
    }
}
