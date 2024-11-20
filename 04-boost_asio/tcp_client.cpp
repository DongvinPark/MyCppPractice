#include <array>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

/*
리눅스나 맥 환경이라면, 아래의 명령어로 실행하면 된다.
g++ -std=c++20 -o executable tcp_client.cpp && ./executable localhost 8554 && rm executable

Visual Studio IDE에서 위와 같이 argv 값을 줘서 실행하고 싶다면,
IDE 창 상단의 프로젝트 > 속성 > 디버깅 > 명령 인수 부분에다가
localhost 8554
라고 입력해주고 확인 눌러서 설정 저장한 다음 실행버튼 누르면 된다.
*/

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: client <host> <port>" << "\n";
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);

        const char* host = argv[2];

        // resolve() 함수 호출 시 첫 인자는 host이고, 두 번째 인자는 포트 번호다.
        tcp::resolver::results_type endpoints = resolver.resolve(argv[1], argv[2]);

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        while (true) {
            std::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof) {
                break; // Connection closed cleanly by peer.
            }
            else if (error) {
                throw boost::system::system_error(error);
            }

            std::cout.write(buf.data(), len);
        }//true

    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    std::cout << "\n";
    return 0;
}// main
