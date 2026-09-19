#include "SdpUtils.hpp"
#include <ranges>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

std::optional<std::string_view> SdpUtils::extract(std::string_view sdp, std::string_view prefix) {
    auto matches = sdp 
        | std::views::split("\r\n"sv)
        | std::views::transform([](auto&& line) {
            return std::string_view(line.begin(), line.end());
        })
        | std::views::filter([prefix](std::string_view line) {
            return line.starts_with(prefix);
        });

    if (auto it = matches.begin(); it != matches.end()) {
        return (*it).substr(prefix.size());
    }
        
    return std::nullopt;
}

std::vector<std::string_view> SdpUtils::extractAll(std::string_view sdp, std::string_view prefix) {
    return sdp 
        | std::views::split("\r\n"sv)
        | std::views::transform([](auto&& line) {
            return std::string_view(line.begin(), line.end());
        })
        | std::views::filter([prefix](std::string_view line) {
            return line.starts_with(prefix);
        })
        | std::views::transform([prefix](std::string_view line) {
            return line.substr(prefix.size());
        })
        | std::ranges::to<std::vector>();
}

std::string SdpUtils::strip(std::string_view sdp, std::string_view prefix) {
    auto filtered = sdp
        | std::views::split("\r\n"sv)
        | std::views::transform([](auto&& line) {
            return std::string_view(line.begin(), line.end());
        })
        | std::views::filter([prefix](std::string_view line) {
            return !line.starts_with(prefix);
        });

    std::string result;
    result.reserve(sdp.size());

    bool first = true;
    for (auto&& line : filtered) {
        if (!first) {
            result.append("\r\n"sv);
        }
        result.append(line);
        first = false;
    }
    return result;
}