#include "Guid.h"


#include <iomanip>
#include <random>
#include <ostream>
#include <sstream>

namespace tirx::utils {

/*
std::array<unsigned char, Uuid::uuid_length_> Uuid::generate_uuid() {
    static std::random_device random_device;
    static std::mt19937 range_generator(random_device());
    std::uniform_int_distribution<int> dist(0x0, 0xFF);
    std::array<unsigned char, uuid_length_> data{};
    for (auto& c : data) {
        auto x = dist(range_generator);
        c = static_cast<unsigned char>(dist(range_generator));
    }
    return data;
}

Uuid::Uuid() : data_{ generate_uuid() } {

}

std::ostream& operator<<(std::ostream& stream, const Uuid& uuid) {
    std::stringstream sstream;
    static const unsigned int dash_mask = 0b1010101000;

    for (size_t i = 0; i < Uuid::uuid_length_; ++i) {
        sstream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uuid.data_[i]);
        if ((1 << i & dash_mask) != 0)
            sstream << "-";
    }
    stream << sstream.str();
    return stream;
}
*/

std::string get_uuid() {
#ifdef _DEBUG
    static int i = 1;
    return std::to_string(i++);
#endif

    static std::random_device dev;
    static std::mt19937 rng(dev());

    std::uniform_int_distribution<int> dist(0, 15);

    const char* v = "0123456789abcdef";
    const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

    std::string res;
    for (int i = 0; i < 16; i++) {
        if (dash[i]) res += "-";
        res += v[dist(rng)];
        res += v[dist(rng)];
    }
    return res;
}

}
