#include <array>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

/*
소켓을 만들고 응답을 읽어들이는 방법이 TCP 때에 비해서
약간 달라진 것 말고는 큰 차이점이 없다.
*/
int main(int argc, char* argv[]) {

    try {
        if (argc != 3) {
            std::cerr << "Usage : client <host> <port>" << "\n";
            return 1;
        }

        boost::asio::io_context io_context;

        udp::resolver resolver(io_context);

        udp::endpoint receiver_endpoint =
            *resolver.resolve(udp::v4(), argv[1], argv[2]).begin();

        udp::socket socket(io_context);
        socket.open(udp::v4());

        std::array<char, 1> send_buf = { {0} };
        socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

        std::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(
            boost::asio::buffer(recv_buf), sender_endpoint
        );

        std::cout.write(recv_buf.data(), len);
        std::cout << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
