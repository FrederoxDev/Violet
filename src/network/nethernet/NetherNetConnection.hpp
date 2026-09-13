#pragma once
#include <rtc/rtc.hpp>

class NetherNetConnection {
    std::shared_ptr<rtc::PeerConnection> mPeerConnection;
};