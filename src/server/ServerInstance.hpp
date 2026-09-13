#pragma once
#include <entt/entt.hpp>
#include <filesystem>
#include <unordered_map>
#include "world/dimension/Dimension.hpp"
#include "network/nethernet/NetherNetServer.hpp"

class ServerInstance {
public:
    ServerInstance(std::filesystem::path root);

    void initialize();
    void tick();

private:
    std::filesystem::path mRoot;
    NetherNetServer mNetherNetServer;
    entt::registry mRegistry;

    uint32_t mNextDimensionId = 0;
    std::unordered_map<DimensionId, Dimension> mDimensions;
};  