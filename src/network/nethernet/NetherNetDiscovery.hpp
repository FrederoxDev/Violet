#pragma once
#include <httplib.h>
#include <thread>
#include "NetherNetHttpServer.hpp"

class NetherNetDiscovery {
public:
    void initialize();
    void start();
    void stop();

private:    
    NetherNetHttpServer mHttpServer;
    std::thread mHttpThread;
};