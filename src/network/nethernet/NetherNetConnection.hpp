#pragma once
#include "rtc/peerconnection.hpp"
#include <rtc/rtc.hpp>

class NetherNetConnection {
public:
    rtc::PeerConnection mPeerConnection;

    NetherNetConnection();
    bool waitForGathering(std::chrono::seconds timeout);

private:
    std::promise<void> mGatheringCompletePromise;
    std::future<void> mGatheringCompleteFuture;
    std::atomic_bool mGatheringComplete{false};
};