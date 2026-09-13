#pragma once
#include "ServerPlatform.hpp"
#include <Windows.h>

class WindowsPlatform : public ServerPlatform {
public:
    std::filesystem::path getServerFolder() const override {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return std::filesystem::path(buffer).parent_path();
    }
};