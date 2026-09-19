#pragma once
#include <future>
#include <mutex>
#include <string>
#include <unordered_map>

class MinecraftIdentityVerifier {
public:
    struct JwkRsa {
        std::string kid;
        std::string n;
        std::string e;
    };

    static bool refreshKeys();
    static std::shared_future<bool> refreshKeysAsync();
    static bool verifyIdentity(const std::string& kid, const std::string& token);

private:
    static std::mutex mKeysMutex;
    static std::unordered_map<std::string, std::string> mKidToPemMap;

    static std::mutex mRefreshMutex;
    static std::shared_future<bool> mKeysRefreshFuture;
};