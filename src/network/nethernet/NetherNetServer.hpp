#pragma once
#include "NetherNetDiscovery.hpp"
#include <filesystem>

class NetherNetServer {
public:
    NetherNetServer(std::filesystem::path root)
        : mDiscovery(root) {}

    void initialize() {
        mDiscovery.initialize();
    }

    void start() {
        mDiscovery.start();
    }

private:
    NetherNetDiscovery mDiscovery;
};