#include "ServerInstance.hpp"

ServerInstance::ServerInstance(std::filesystem::path root)
    : mRoot(std::move(root)) {}

void ServerInstance::initialize() {
    DimensionId overworldId{ mNextDimensionId++ };

    mDimensions.emplace(
        overworldId.value, 
        Dimension(overworldId, "minecraft:overworld")
    );

    mNetherNetServer.initialize();
    mNetherNetServer.start();
}   

void ServerInstance::tick() {}