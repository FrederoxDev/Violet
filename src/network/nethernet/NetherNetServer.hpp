#pragma once
#include "NetherNetDiscovery.hpp"
#include "network/nethernet/NetherNetConnection.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>

class NetherNetServer {
public:
    NetherNetServer() : mDiscovery(*this) {}

    void initialize() {
        mDiscovery.initialize();
    }

    void start() {
        mDiscovery.start();
    }

    std::optional<std::shared_ptr<NetherNetConnection>> createConnection(std::string networkId);

private:
    std::mutex mConnectionsMutex;
    std::unordered_map<std::string, std::shared_ptr<NetherNetConnection>> mConnections;

    NetherNetDiscovery mDiscovery;
};