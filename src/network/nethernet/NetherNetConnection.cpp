#include "NetherNetConnection.hpp"

NetherNetConnection::NetherNetConnection()
    : mGatheringCompleteFuture(mGatheringCompletePromise.get_future()) {
        mPeerConnection.onGatheringStateChange([&](auto state) {
            if (state == rtc::PeerConnection::GatheringState::Complete && !mGatheringComplete.exchange(true)) {
                mGatheringCompletePromise.set_value();        
            }
        });
    }

bool NetherNetConnection::waitForGathering(std::chrono::seconds timeout) {
    return mGatheringCompleteFuture.wait_for(timeout) == std::future_status::ready;
}