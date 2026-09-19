#include "NetherNetDiscovery.hpp"
#include "network/nethernet/NetherNetConnection.hpp"
#include "rtc/description.hpp"
#include "rtc/peerconnection.hpp"
#include <chrono>
#include <print>
#include <thread>
#include "network/nethernet/NetherNetServer.hpp"
#include "network/nethernet/identity/OfferIdentity.hpp"
#include "network/nethernet/identity/ClientIdentity.hpp"

void NetherNetDiscovery::initialize() {
    mHttpServer.Get("/v1/join",[](const httplib::Request& req, httplib::Response& res) {
        res.set_content(
            R"({"name": "NetherNet Server", "protocol": 2169, "version": "1.26.45", "level": "Bedrock level", "players": 67, "maxPlayers": 100, "gameType": 1})", 
            "application/json"
        );
    });

    mHttpServer.Post("/v1/join/:networkId", [this](const httplib::Request& req, httplib::Response& res) {
        auto networkId = req.path_params.at("networkId");
        auto offerIdentityResult = OfferIdentity::extractOfferIdentity(req.body);
        if (!offerIdentityResult) {
            res.status = 401;
            return;
        }

        auto verifiedIdentity = offerIdentityResult->verify();
        if (!verifiedIdentity) {
            res.status = 401;
            return;
        }       
        
        std::println("Creating connection for networkId: {}, clientIdentity: name={}, xuid={}, playfabId={}", 
            networkId, 
            verifiedIdentity->name, 
            verifiedIdentity->xuid, 
            verifiedIdentity->playfabId
        );

        auto conResult = this->mServer.createConnection(networkId);
        if (!conResult.has_value()) {
            res.status = 400;
            return;
        }

        auto& clientIdentity = offerIdentityResult.value();
        auto& connection = *conResult.value();
        auto& peer = connection.mPeerConnection;

        peer.setRemoteDescription(rtc::Description(
            clientIdentity.strippedSdp,
            rtc::Description::Type::Offer)
        );
        
        peer.setLocalDescription(rtc::Description::Type::Answer);

        if (!connection.waitForGathering(std::chrono::seconds(5))) {
            res.status = 500;
            std::println("timeout for networkId: {}", networkId);
            return;
        }

        auto description = peer.localDescription();
        if (!description.has_value()) {
            std::println("Failed to get local description for networkId: {}", networkId);
            res.status = 500;
            return;
        }

        res.set_content(
            std::string(description.value()),
            "application/sdp"
        );

        std::println("Sent answer for networkId: {}, description: {}", networkId, std::string(*description));
    });
}

void NetherNetDiscovery::start() {
    std::println("Starting NetherNet discovery server on port 19132...");

    mHttpThread = std::thread([this]() {
        mHttpServer.listen("0.0.0.0", 19132);
    });
}

void NetherNetDiscovery::stop() {
    std::println("Stopping NetherNet discovery server...");
    mHttpServer.stop();

    if (mHttpThread.joinable()) {
        mHttpThread.join();
    }
}