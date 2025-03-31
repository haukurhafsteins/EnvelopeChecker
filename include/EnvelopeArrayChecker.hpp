#pragma once
#include "EnvelopBase.hpp"

template<typename T>
struct EnvelopeArrayConfig : public EnvelopeBaseConfig<T> {
    T maxAllowedFailureRatio = T(0); // 0.1 = 10%
};

template<typename T>
class EnvelopeArrayChecker {
public:
    void setConfig(const EnvelopeArrayConfig<T>& cfg) { config = cfg; }

    void setReference(const std::vector<T>& refArray) {
        reference = refArray;
        violationTimer = T(0);
        lastStatus = EnvelopeArrayStatus::OK;
    }

    EnvelopeArrayStatus check(const std::vector<T>& sample) {
        if (sample.size() != reference.size())
            throw std::runtime_error("Sample size mismatch");

        above = 0, below = 0;

        for (size_t i = 0; i < sample.size(); ++i) {
            T upperMargin = computeEnvelopeMargin(config, reference[i], config.marginUpper);
            T lowerMargin = computeEnvelopeMargin(config, reference[i], config.marginLower);
            T upper = std::min(reference[i] + upperMargin, config.clampMax);
            T lower = std::max(reference[i] - lowerMargin, config.clampMin);

            if (sample[i] > upper) ++above;
            else if (sample[i] < lower) ++below;
        }

        const double failRatio = static_cast<T>(above + below) / static_cast<T>(sample.size());
        if (failRatio > config.maxAllowedFailureRatio) {
            lastStatus = (above >= below)
                       ? EnvelopeArrayStatus::AnyAboveUpperLimit
                       : EnvelopeArrayStatus::AnyBelowLowerLimit;
        } else {
            lastStatus = EnvelopeArrayStatus::OK;
        }

        return lastStatus;
    }

    std::vector<size_t> getFailedBinIndices(std::span<const T> sample) const {
        if (sample.size() != reference.size())
            throw std::runtime_error("Sample size mismatch");
    
        std::vector<size_t> failed;
    
        for (size_t i = 0; i < sample.size(); ++i) {
            T upperMargin = computeEnvelopeMargin(config, reference[i], config.marginUpper);
            T lowerMargin = computeEnvelopeMargin(config, reference[i], config.marginLower);
            T upper = std::min(reference[i] + upperMargin, config.clampMax);
            T lower = std::max(reference[i] - lowerMargin, config.clampMin);
    
            if (sample[i] > upper || sample[i] < lower) {
                failed.push_back(i); // Store failing bin index
            }
        }
    
        return failed;
    }
    
    void getFailureRatio(double& above, double& below) const {
        size_t total = reference.size();
        above = static_cast<double>(above) / total;
        below = static_cast<double>(below) / total;
    }

    EnvelopeArrayStatus update(T deltaTime) {
        if (lastStatus == EnvelopeArrayStatus::OK) {
            violationTimer = T(0);
        } else {
            violationTimer += deltaTime;
            if (violationTimer >= config.violationTimeout && config.violationTimeout > T(0)) {
                return EnvelopeArrayStatus::ViolationTimeout;
            }
        }
        return lastStatus;
    }

    EnvelopeArrayStatus getStatus() const { return lastStatus; }

private:
    EnvelopeArrayConfig<T> config{};
    std::vector<T> reference{};
    T violationTimer = T(0);
    size_t above = 0, below = 0; 
    EnvelopeArrayStatus lastStatus = EnvelopeArrayStatus::OK;
};
