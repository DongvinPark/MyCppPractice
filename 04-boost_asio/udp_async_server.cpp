#include <array>
#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm time_info;

#ifdef _WIN32
    localtime_s(&time_info, &now_c); // Windows-safe version
#else
    time_info = *std::localtime(&now_c); // Linux and POSIX systems
#endif

    std::ostringstream oss;
    oss << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

class udp_server {
public:
    udp_server(boost::asio::io_context& io_context) :
        socket_(io_context, udp::endpoint(udp::v4(), 8554)) {
        start_receive();
    }

private:
    void start_receive() {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_),
            remote_endpoint_,
            [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                handle_receive(error, bytes_transferred);
            }
        );
    }

    void handle_receive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
        if (!error) {
            std::shared_ptr<std::string> message(new std::string(make_daytime_string()));

            socket_.async_send_to(
                boost::asio::buffer(*message),
                remote_endpoint_,
                [this, message](const boost::system::error_code& error, std::size_t bytes_transferred) {
                    handle_send(message, error, bytes_transferred);
                }
            );

            start_receive();
        }
    }

    void handle_send(
        std::shared_ptr<std::string> /*message*/,
        const boost::system::error_code& /*error*/,
        std::size_t /*bytes_transferred*/
    ) {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1> recv_buffer_;
};

int main() {

    try {
        boost::asio::io_context io_context;
        udp_server server(io_context);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
