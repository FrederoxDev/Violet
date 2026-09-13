#pragma once
#include <httplib.h>
#include <thread>

class NetherNetDiscovery {
public:
    NetherNetDiscovery(std::filesystem::path root);

    void initialize();
    void start();
    void stop();

private:
    std::string mCertPath;
    std::string mKeyPath;
    
    httplib::SSLServer mHttpServer;
    std::thread mHttpThread;
};