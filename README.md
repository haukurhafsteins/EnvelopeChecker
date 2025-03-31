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

---

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
```

The array checker extends it:

```cpp
template<typename T>
struct EnvelopeArrayConfig : EnvelopeBaseConfig<T> {
    T maxAllowedFailureRatio;      // e.g., 0.1 = 10% bins may fail
};
```

---

## ✅ Scalar Usage

```cpp
EnvelopeChecker<float> checker;

EnvelopeBaseConfig<float> config;
config.mode = EnvelopeMarginMode::Percent;
config.marginUpper = 10.0f;
config.marginLower = 5.0f;
config.violationTimeout = 0.2f; // seconds

checker.setConfig(config);
checker.setReference(100.0f);

auto status = checker.check(105.5f);  // Immediate envelope status
status = checker.update(0.01f);       // Call periodically with delta time
```

---

## 📊 Array Usage (e.g. PSD signal)

```cpp
EnvelopeArrayChecker<float> psdChecker;

EnvelopeArrayConfig<float> config;
config.mode = EnvelopeMarginMode::Percent;
config.marginUpper = 15.0f;
config.marginLower = 5.0f;
config.maxAllowedFailureRatio = 0.1f; // 10% of bins allowed to fail
config.violationTimeout = 0.5f;

psdChecker.setConfig(config);
psdChecker.setReference(baselinePsd);

auto status = psdChecker.check(currentPsd);
status = psdChecker.update(0.02f);
```

### 🔍 Per-bin Diagnostics

```cpp
auto failedBins = psdChecker.getFailedBinIndices(currentPsd);
for (size_t i : failedBins) {
    std::cout << "Bin " << i << " failed: " << currentPsd[i] << "\n";
}
```

Or to get a failure mask:

```cpp
auto mask = psdChecker.getFailureMask(currentPsd);
```

---

## 📌 EnvelopeStatus Enums

Scalar:

```cpp
enum class EnvelopeStatus {
    OK,
    AboveUpperLimit,
    BelowLowerLimit,
    ViolationTimeout
};
```

Array:

```cpp
enum class EnvelopeArrayStatus {
    OK,
    AnyAboveUpperLimit,
    AnyBelowLowerLimit,
    ViolationTimeout
};
```

---

## 🔧 Extending Ideas

- Support for fixed-size arrays with `std::array<T, N>` or `std::span<T, N>`
- Add real-time smoothing or decay for adaptive envelopes
- Log violation history or statistics
- JSON configuration for UIs or CLI tools
- Embedded-friendly version with no exceptions

---

## 📜 License

MIT — use freely in personal or commercial projects.

---

## 🤝 Contributing

Feel free to open issues or PRs for improvements, especially:

- C++20 optimization
- Custom comparator policies
- Visualization tooling integration
