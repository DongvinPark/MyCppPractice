#include <iostream>
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

/*
TCP Sync Server와 크게 달라진 건 없다.
예측한 대로, 

UDP sync server is listening on port 8554 ...
New request arrives!
Wrote response to client in sync manner!
New request arrives!
Wrote response to client in sync manner!

와 같이 순차적이고, blocking 되면서 작동한다.
*/
int main() {

    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), 8554));

        std::cout << "UDP sync server is listening on port 8554 ...\n";

        while (true) {
            std::array<char, 1> recv_buf;
            udp::endpoint remote_endpoint;
            socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
            std::cout << "New request arrives!\n";

            std::string message = make_daytime_string();

            boost::system::error_code ignored_error;
            socket.send_to(
                boost::asio::buffer(message), remote_endpoint, 0, ignored_error
            );
            std::cout << "Wrote response to client in sync manner!\n";
        }//wh

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
