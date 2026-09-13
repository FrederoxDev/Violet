#pragma once
#include <httplib.h>
#include <thread>
#include "NetherNetHttpServer.hpp"

class NetherNetServer;

class NetherNetDiscovery {
public:
    void initialize();
    void start();
    void stop();

    NetherNetDiscovery(NetherNetServer& server) 
        : mServer(server) {}

private:    
    NetherNetServer& mServer;
    NetherNetHttpServer mHttpServer;
    std::thread mHttpThread;
};