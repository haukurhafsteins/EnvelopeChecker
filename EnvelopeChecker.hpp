#pragma once
#include "EnvelopBase.hpp"

template<typename T>
class EnvelopeChecker {
public:
    void setConfig(const EnvelopeBaseConfig<T>& cfg) { config = cfg; }

    void setReference(T ref) {
        reference = ref;
        violationTimer = T(0);
        lastStatus = EnvelopeStatus::OK;
    }

    EnvelopeStatus check(T value) {
        T upperMargin = computeEnvelopeMargin(config, reference, config.marginUpper);
        T lowerMargin = computeEnvelopeMargin(config, reference, config.marginLower);

        T upper = std::min(reference + upperMargin, config.clampMax);
        T lower = std::max(reference - lowerMargin, config.clampMin);

        if (value > upper)
            lastStatus = EnvelopeStatus::AboveUpperLimit;
        else if (value < lower)
            lastStatus = EnvelopeStatus::BelowLowerLimit;
        else
            lastStatus = EnvelopeStatus::OK;

        return lastStatus;
    }

    EnvelopeStatus update(T deltaTime) {
        if (lastStatus == EnvelopeStatus::OK) {
            violationTimer = T(0);
        } else {
            violationTimer += deltaTime;
            if (violationTimer >= config.violationTimeout && config.violationTimeout > T(0)) {
                return EnvelopeStatus::ViolationTimeout;
            }
        }
        return lastStatus;
    }

    std::pair<T, T> getEnvelopeBounds() const {
        T upperMargin = computeEnvelopeMargin(config, reference, config.marginUpper);
        T lowerMargin = computeEnvelopeMargin(config, reference, config.marginLower);
        return {
            std::max(reference - lowerMargin, config.clampMin),
            std::min(reference + upperMargin, config.clampMax)
        };
    }
    
    EnvelopeStatus getStatus() const { return lastStatus; }

private:
    EnvelopeBaseConfig<T> config{};
    T reference{};
    T violationTimer = T(0);
    EnvelopeStatus lastStatus = EnvelopeStatus::OK;
};
