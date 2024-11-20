#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

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

int main() {
    try {
        int port = 8554;
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
        std::cout << "Tcp Sync Datatime Server is Listening on " << port << "\n";

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "client connected!\n";

            std::string message = make_daytime_string();
            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);

            std::cout << "writing response completed!\n";
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }
    return 0;
}

