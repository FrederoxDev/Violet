#pragma once
#include <string_view>
#include <optional>
#include <string>
#include <vector>

class SdpUtils {
public:
    static std::optional<std::string_view> extract(std::string_view sdp, std::string_view prefix);
    static std::vector<std::string_view> extractAll(std::string_view sdp, std::string_view prefix);
    static std::string strip(std::string_view sdp, std::string_view prefix);
};