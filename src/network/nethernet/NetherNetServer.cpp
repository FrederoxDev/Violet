#include "NetherNetServer.hpp"
#include <memory>

std::optional<std::shared_ptr<NetherNetConnection>> NetherNetServer::createConnection(std::string networkId) {
    std::lock_guard<std::mutex> lock(mConnectionsMutex);

    if (mConnections.find(networkId) != mConnections.end()) {
        return std::nullopt;
    }

    auto connection = std::make_shared<NetherNetConnection>();

    mConnections.emplace(
        std::move(networkId),
        connection
    );

    return connection;
}