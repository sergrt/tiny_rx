#pragma once

#include <array>
#include <string>

namespace tiny_rx {

class Guid {
public:
    Guid();
    std::string to_string() const;
    bool operator==(const Guid& other) const;

private:
    std::array<uint8_t, 16> data_{};
};

} // namespace tiny_rx
