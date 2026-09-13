#pragma once
#include "httplib.h"

class NetherNetHttpServer : public httplib::Server {
private:
    bool process_and_close_socket(socket_t sock) override {
        if (httplib::detail::select_read(
                sock,
                read_timeout_sec_,
                read_timeout_usec_) <= 0) {
            httplib::detail::shutdown_socket(sock);
            httplib::detail::close_socket(sock);
            return false;
        }

        unsigned char firstByte = 0;

        const auto received = ::recv(
            sock,
            reinterpret_cast<char*>(&firstByte),
            1,
            MSG_PEEK
        );

        if (received <= 0) {
            httplib::detail::shutdown_socket(sock);
            httplib::detail::close_socket(sock);
            return false;
        }

        // Force the client to fall back to plain HTTP since otherwise it'll timeout before it tries HTTP :(
        if (firstByte == 0x16) {
            httplib::detail::shutdown_socket(sock);
            httplib::detail::close_socket(sock);
            return false;
        }

        // Impl of httplib::Server::process_and_close_socket
        // original func was private so had to copy here

        std::string remoteAddr;
        int remotePort = 0;
        httplib::detail::get_remote_ip_and_port(
            sock, remoteAddr, remotePort
        );

        std::string localAddr;
        int localPort = 0;
        httplib::detail::get_local_ip_and_port(
            sock, localAddr, localPort
        );

        bool websocketUpgraded = false;

        const auto result = httplib::detail::process_server_socket(
            svr_sock_,
            sock,
            keep_alive_max_count_,
            keep_alive_timeout_sec_,
            read_timeout_sec_,
            read_timeout_usec_,
            write_timeout_sec_,
            write_timeout_usec_,
            [&](httplib::Stream& stream,
                bool closeConnection,
                bool& connectionClosed) {

                return process_request(
                    stream,
                    remoteAddr,
                    remotePort,
                    localAddr,
                    localPort,
                    closeConnection,
                    connectionClosed,
                    nullptr,
                    &websocketUpgraded
                );
            }
        );

        httplib::detail::shutdown_socket(sock);
        httplib::detail::close_socket(sock);

        return result;
    }
};