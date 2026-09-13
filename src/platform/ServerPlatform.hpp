#pragma once

#include <filesystem>
class ServerPlatform {
public:
    virtual std::filesystem::path getServerFolder() const = 0;
};