#pragma once

#include <array>
#include <random>
#include <string>

namespace tirx::utils {
/*
class Uuid {
public:
    Uuid();

    friend std::ostream& operator<<(std::ostream& stream, const Uuid& uuid);

private:
    static const size_t uuid_length_ = 16;

    static std::array<unsigned char, uuid_length_> generate_uuid();
    std::array<unsigned char, uuid_length_> data_{};
};
*/

std::string get_uuid();

}
