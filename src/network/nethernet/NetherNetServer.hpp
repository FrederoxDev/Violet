#pragma once
#include "NetherNetDiscovery.hpp"
#include <filesystem>

class NetherNetServer {
public:
    void initialize() {
        mDiscovery.initialize();
    }

    void start() {
        mDiscovery.start();
    }

private:
    NetherNetDiscovery mDiscovery;
};