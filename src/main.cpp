#include <chrono>
#include <entt/entt.hpp>
#include <thread>
#include <print>

#include "platform/WindowsPlatform.hpp"
#include "server/ServerInstance.hpp"

constexpr int TPS = 20;
constexpr float DT = 1.0f / TPS;

int main() {
    WindowsPlatform platform = WindowsPlatform();

    ServerInstance server{platform.getServerFolder()};
    server.initialize();

    const auto tickDuration = std::chrono::duration<double>(1.0 / TPS);
    auto nextTick = std::chrono::steady_clock::now();

    while (true) {
        server.tick();

        nextTick += std::chrono::duration_cast<std::chrono::steady_clock::duration>(tickDuration);

        std::this_thread::sleep_until(nextTick);
    }

    return 0;
}
