#include "Guid.h"

#include <random>
#include <stdexcept>

namespace tiny_rx {

Guid::Guid() {

#ifdef _DEBUG
    static int id = 1;
    *reinterpret_cast<int*>(data_.data()) = id++;
    return;
#endif

    static std::random_device dev;
    static std::mt19937 rng(dev());

    std::uniform_int_distribution<int> dist(0, 255);

    for (auto& i : data_) {
        i = dist(rng);
    }
}

std::string Guid::to_string() const {
    static constexpr bool kDash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

    std::string res;
    res.reserve(36);
    static constexpr int kConvertBufferSize = 3;
    char convertBuffer[kConvertBufferSize] {};
    for (int i = 0; i < 16; i++) {
        if (kDash[i])
            res += "-";
        if (snprintf(convertBuffer, kConvertBufferSize, "%02X", data_[i]) != kConvertBufferSize - 1) {
            throw(std::runtime_error("Unable to convert guid"));
        } else {
            res += std::string(convertBuffer);
        }
    }
    return res;
}

bool Guid::operator==(const Guid& other) const {
    return data_ == other.data_;
}

} // namespace tiny_rx
