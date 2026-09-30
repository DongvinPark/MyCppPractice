#include <boost/redis/src.hpp> // 이게 반드시 있어야 한다!!

#include <boost/redis/connection.hpp>
#include <boost/redis/push_parser.hpp>

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/consign.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>

#include <iostream>

/*
 * Boost Redis 와 coroutine 을 활용해서 Redis Pub/Sub 메시지 리스너를 구현한 아래의 예제를 참고했다.
 * https://github.com/boostorg/redis/blob/master/example/cpp20_subscriber.cpp#L68
 * C++20 또는 그 이상 버전과 boost 1.92 버전 혹은 그 이상이 필요하며,
 * 로컬 머신에서 redis가 6379 포트로 돌고 있어야 한다.
 *
 * 이 예제는 레디스 Pub/Sub 채널을 계속 리스닝 하고 있는다.
 * 레디스 pub/sub 채널에 메시지를 푸시하려면 아래의 명령어를 쓰면 된다.
 * $ redis-cli PUBLISH chat "hello redis PubSub listener in C++20"
 *
 * 이 명령을 실행해보면 레디스에서 pub/sub 채널 리스너들의 연결을 전부 끊는다.
 * 그로 인해 에러가 뜨더라도 본 예제에서 다시 연결해서 리스닝 하고 있는 것을 확인할 수 있다.
 * $ redis-cli CLIENT kill TYPE pubsub
 */

#if defined(BOOST_ASIO_HAS_CO_AWAIT)

// 레디스에 푸시된 메시지를 리스닝한다.
auto pub_sub_listener(
  std::shared_ptr<boost::redis::connection> conn) -> boost::asio::awaitable<void>
{
  boost::redis::generic_flat_response resp;
  conn->set_receive_response(resp);

  // 채널을 구독한다. 여러개 구독할 수도 있다.
  boost::redis::request req;
  req.subscribe({"chat"});
  co_await conn->async_exec(req);

  // 채널(또는 채널들) 구독이 완료됐다. 채널에 푸시된 메시지들은 resp에 쌓인다.
  // 커넥션이 네트워크 에러 떠서 레디스에 다시 연결할 때, 채널들을 자동으로 다시 구독한다.
  // 그러기 위해서는 request::subscribe()를 호출해야 한다.
  while (conn->will_reconnect())
  {
    // 메시지 도착을 기다린다.
    auto [ec] = co_await conn->async_receive2(boost::asio::as_tuple);

    // 에러 체크.
    if (ec)
    {
      std::cerr << "Error during receive: " << ec << "\n";
      break;
    }

    // 권한 부족 등의 이유로 아래의 에러체크문이 실행될 수도 있다.
    if (ec)
    {
      std::cerr << "The receive response contains an error: "
        << resp.error().diagnostic << "\n";
      break;
    }

    // 받은 응답은 코루틴을 suspend 하지 않으면서 즉각 소비돼야 한다. 즉, async operation 으로 소비하면 안 된다.
    for (boost::redis::push_view elem : boost::redis::push_parser(resp.value()))
    {
      std::cout << "Received message from channel " << elem.channel
        << ": " << elem.payload << "\n";
    }

    resp.value().clear();
  } //wh
} //end of


auto co_entry(boost::redis::config cfg) -> boost::asio::awaitable<void>
{
  auto ex = co_await boost::asio::this_coro::executor;
  auto conn = std::make_shared<boost::redis::connection>(ex);
  co_spawn(ex, pub_sub_listener(conn), boost::asio::detached);
  conn->async_run(
    cfg,
    boost::asio::consign(boost::asio::detached, conn)
  );

  boost::asio::signal_set sig_set(ex, SIGINT, SIGTERM);
  co_await sig_set.async_wait();

  conn->cancel();
}

#endif // defined(BOOST_ASIO_HAS_CO_AWAIT)


int main()
{
  boost::asio::io_context io_context;

  boost::redis::config cfg;
  cfg.addr.host = "127.0.0.1";
  cfg.addr.port = "6379";

  boost::asio::co_spawn(
    io_context,
    co_entry(cfg),
    boost::asio::detached
  );

  io_context.run();

  return 0;
}
