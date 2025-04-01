#pragma once
#include <span>
#include "../components/EnvelopeChecker/include/EnvelopeBase.hpp"

template<typename T>
struct EnvelopeArrayConfig : public EnvelopeBaseConfig<T> {
    T maxAllowedFailureRatio = T(0); // 0.1 = 10%
};

template<typename T>
class EnvelopeArrayChecker {
public:
    void setConfig(const EnvelopeArrayConfig<T>& cfg) { config = cfg; }

    void setReferenceBuffer(T* buffer, size_t size) {
        referenceBuffer = std::span<T>(buffer, size);
        reference = referenceBuffer;
    }

    size_t getReferenceBufferSize() const {
        return referenceBuffer.size();
    }

    bool setReferenceValues(std::span<const T> values) {
        if (values.size() != referenceBuffer.size()) {
            return false; // Size mismatch
        }
        std::copy(values.begin(), values.end(), referenceBuffer.begin());
        return true;
    }

    EnvelopeArrayStatus check(const std::span<const T>& sample) {
        if (reference.empty())
            return EnvelopeArrayStatus::NoReferenceSet;
        if (sample.size() != reference.size())
            return EnvelopeArrayStatus::SizeMismatch;

        above = 0, below = 0;

        for (size_t i = 0; i < sample.size(); ++i) {
            T upper, lower;
            calculateMargin(reference[i], upper, lower);

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

    EnvelopeArrayStatus check(const std::vector<T>& sample) {
        return check(std::span<const T>(sample));
    }

    std::vector<size_t> getFailedBinIndices(std::span<const T> sample) const {
        if (sample.size() != reference.size())
            throw std::runtime_error("Sample size mismatch");
    
        std::vector<size_t> failed;
    
        for (size_t i = 0; i < sample.size(); ++i) {
            T upper, lower;
            calculateMargin(reference[i], upper, lower);
    
            if (sample[i] > upper || sample[i] < lower) {
                failed.push_back(i); // Store failing bin index
            }
        }
    
        return failed;
    }

    bool getEnvelope(std::span<T> outHigh, std::span<T> outLow, size_t size) const {
        if (size != reference.size()) {
            printf("!!! Warning: Size mismatch in getEnvelope !!!\n");
            return false; // Size mismatch
        }
        for (size_t i = 0; i < size; ++i) {
            T upper, lower;
            calculateMargin(reference[i], upper, lower);
            outHigh[i] = upper;
            outLow[i] = lower;
        }
        return true;
    }
    
    void getFailureRatio(double& above, double& below) const {
        double total = reference.size();
        if (total == 0) {
            printf("!!! Warning: No reference values set !!!\n");
            above = below = 0;
            return;
        }
        above = static_cast<double>(this->above) / total;
        below = static_cast<double>(this->below) / total;
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
    void calculateMargin(T ref, T& upper, T& lower) const {
        upper = std::min(ref + computeEnvelopeMargin(config, ref, config.marginUpper), config.clampMax);
        lower = std::max(ref - computeEnvelopeMargin(config, ref, config.marginLower), config.clampMin);
    }

    EnvelopeArrayConfig<T> config{};
    std::span<T> reference;     // Points to actual ref values used in checking
    std::span<T> referenceBuffer;   // Memory owned by caller, but managed here
    T violationTimeout = T(0);
    T violationTimer = T(0);
    size_t above = 0, below = 0; 
    EnvelopeArrayStatus lastStatus = EnvelopeArrayStatus::OK;
};
