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

	// 운영체제에 맞는 함수를 사용하게 만든다.
#ifdef _WIN32
	localtime_s(&time_info, &now_c); // Windows-safe version
#else
	time_info = *std::localtime(&now_c); // Linux and POSIX systems
#endif

	std::ostringstream oss;
	oss << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S");

	return oss.str();
}

// tcp_connection 객체가 
// public std::enable_shared_from_this<tcp_connection> 객체를 상속하게 만듦으로써,
// tcp_connection 객체가 자기 자신을 가리키는 shared_ptr를 안전하게 만들 수 있게 해준다.
// 이렇게 만들어진 'tcp_connection 자신에 대한 shared_ptr'는 boost.asio의 io_context에게
// 전달할 completion handler 인 lambda의 캡쳐 리스트에 전달되며, 이로써
// 해당 lambda는 boost.asio에게 전달된 이후에도 안전하게 자기가 원래 참조해야 하는
// tcp_connection 객체를 참조할 수 있게 된다.
class tcp_connection : public std::enable_shared_from_this<tcp_connection>{
public:
	// tcp_connection 객체는 현재의 async 서버 코드 뿐만 아니라, boost.asio 라이브러리 코드에서도
	// 사용하기 때문에 두 주체가 소유권을 공유할 수 있게 해줌과 동시에 수명도 자동으로 관리할 수 있게 해주는
	// shated_ptr를 사용하기에 적합하다.
	typedef std::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_context& io_context) {
		return pointer(new tcp_connection(io_context));
	}

	tcp::socket& socket() {
		return socket_;
	}

	void start() {
		message_ = make_daytime_string();

		auto self = shared_from_this();
		boost::asio::async_write(
			socket_,
			boost::asio::buffer(message_),

			[self](const boost::system::error_code& error, size_t bytes_transferred) {
				self->handle_write(error, bytes_transferred);
			}
		);
		std::cout << "Wrote response in aysnc manner.\n";
	}

private:
	tcp_connection(boost::asio::io_context& io_context) : socket_(io_context) {}

	void handle_write(const boost::system::error_code& /*error*/, size_t /*byte_transferred*/) {}

	tcp::socket socket_;
	std::string message_;
};

class tcp_server {
public:
	tcp_server(boost::asio::io_context& io_context) : 
		io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 8554)) {
		std::cout << "Tcp Async Server started on port 8554...\n";
		start_accept();
	}

private:
	void start_accept() {
		auto new_connection = tcp_connection::create(io_context_);

		// 현재 서버 로직을 blocking하지 않으면서 동시에 서버가 다음 요청을 기다릴 수 있게 해준다.
		// 자세히 보면, handle_accept()가 start_accept()를 호출하고,
		// start_accept()가 async 하게 handle_accept()를 호출하여 재귀적 호출 구조로 
		// async 서버가 마치 while(true){...}가 작동하는 것처럼 요청 수신 루프를 반복하게 해준다.
		std::cout << "Waiting for new requests...\n";
		acceptor_.async_accept(
			new_connection->socket(),
			[this, new_connection](const boost::system::error_code& error) {
				handle_accept(new_connection, error);
			}
		);
	}

	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error) {
		if (!error) {
			new_connection->start();
		}
		start_accept();
	}

	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor_;
};

int main() {
	try {
		boost::asio::io_context io_context;
		tcp_server server(io_context);
		io_context.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << "\n";
	}
	return 0;
}
