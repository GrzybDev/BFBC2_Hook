#pragma once

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
	WebSocketClient(net::io_context& ioc, ssl::context& ctx, bool isSecure);

	void Connect(const std::string& host, const USHORT& port, const std::string& target);

private:
	tcp::resolver resolver_;
	websocket::stream<tcp::socket> ws_;
	websocket::stream<beast::ssl_stream<tcp::socket>> wss_;
	beast::flat_buffer buffer_;

	bool isSecure_;

	void OnRead(const beast::error_code& ec, std::size_t bytesTransferred);
};
