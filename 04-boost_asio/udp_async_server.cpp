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
        std::cout << "Async UDP Daytime Server is running of port 8554 ...\n";
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

            // 여기에서 다시 start_receive()를 호출하기 때문에 결과적으로 재귀에 의한 루프가 형성된다.
            start_receive();
        }
    }

    void handle_send(
        std::shared_ptr<std::string> /*message*/,
        const boost::system::error_code& error,
        std::size_t bytes_transferred
    ) {
        if (error) {
            std::cerr << "Write failed: " << error.message() << "\n";
        } else {
            std::cout << "Successfully wrote " << bytes_transferred << " bytes in async manner.\n";
        }
    }

    /*
     똑같은 소켓을 가지고 요청을 하는 모든 클라이언트에게 응답을 전송하는 것에 사용한다.
     그렇기 때문에, 각 요청마다 새로운 소캣을 만들어줘야 하는 TCP 서버보다 구현이 간단해진다.
    */
    udp::socket socket_;
    // remote_endpoint 또한 계속 재사용된다.
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

