# EnvelopeChecker

A modern, flexible C++ header-only library for envelope checking of scalar and array-based signals.  
Useful for signal monitoring, quality assurance, vibration analysis, PSD spectrum checking, and embedded system diagnostics.

---

## ✨ Features

- Envelope-based signal comparison (absolute or percentage-based margins)
- Configurable clamping, minimum margin, and violation timeout
- Supports both **scalar** and **array/vector** signals
- Tolerance via **max allowed failure ratio** (for array version)
- Optional failure histogram / bin tracking
- Clean, modern C++17/20 style
- `std::span` support for efficient buffer passing

---

## 📦 Files

| File                      | Purpose                          |
|---------------------------|----------------------------------|
| `EnvelopeBase.hpp`        | Common enums and config structs  |
| `EnvelopeChecker.hpp`     | Scalar signal checker            |
| `EnvelopeArrayChecker.hpp`| Array/vector signal checker      |

---\

## ⚙️ Configuration

All checkers use a shared base configuration:

```cpp
template<typename T>
struct EnvelopeBaseConfig {
    EnvelopeMarginMode mode;        // Absolute or Percent
    T marginUpper;                  // Upper envelope margin
    T marginLower;                  // Lower envelope margin
    T clampMin;                     // Hard minimum clamp
    T clampMax;                     // Hard maximum clamp
    T minMargin;                    // Smallest allowed margin
    T violationTimeout;            // Time (seconds) signal can stay out of envelope
};
