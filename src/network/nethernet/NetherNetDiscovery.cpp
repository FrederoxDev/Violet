#include "NetherNetDiscovery.hpp"
#include <print>
#include <thread>

void NetherNetDiscovery::initialize() {
    mHttpServer.Get("/v1/join",[](const httplib::Request& req, httplib::Response& res) {
        res.set_content(
            R"({"name": "NetherNet Server", "protocol": 2169, "version": "1.26.45", "level": "Bedrock level", "players": 10, "maxPlayers": 100, "gameType": 1})", 
            "application/json"
        );

        std::println("Received join request from {}:{} body: {}", req.remote_addr, req.remote_port, req.body);
    });

    mHttpServer.Post("/v1/join/:networkId", [](const httplib::Request& req, httplib::Response& res) {
        auto networkId = req.path_params.at("networkId");

        std::println("Received join request for network ID {} from {}:{} body: {}", networkId, req.remote_addr, req.remote_port, req.body);
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