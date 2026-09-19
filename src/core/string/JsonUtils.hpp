#pragma once
#include <string_view>
#include <optional>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonUtils {
public:
    static std::optional<json> parseObject(std::string_view jsonString) {
        json res = json::parse(jsonString, nullptr, false);

        if (res.is_discarded() || !res.is_object()) {
            return std::nullopt;
        }

        return res;
    }

    static std::optional<std::string> getString(
        const json& obj,
        std::string_view key
    ) {
        auto it = obj.find(key);
        if (it == obj.end() || !it->is_string()) {
            return std::nullopt;
        }
        return it->get<std::string>();
    }

    static std::optional<json> getStringifiedObject(
        const json& obj,
        std::string_view key
    ) {
        auto it = obj.find(key);

        if (it == obj.end() || !it->is_string()) {
            return std::nullopt;
        }

        return JsonUtils::parseObject(it->get_ref<const std::string&>());
    }

    static std::optional<std::vector<json>> getObjectArray(const json& obj, std::string_view key) {
        auto it = obj.find(key);
        if (it == obj.end() || !it->is_array()) {
            return std::nullopt;
        }
        
        std::vector<json> result;
        for (const auto& item : *it) {
            if (!item.is_object()) {
                return std::nullopt;
            }
            result.push_back(item);
        }
        
        return result;
    }
};