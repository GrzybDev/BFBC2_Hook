#pragma once

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class HttpClient : public std::enable_shared_from_this<HttpClient>
{
public:
	HttpClient(net::io_context& ioc, ssl::context& ctx, bool isSecure);

	http::response<http::dynamic_body> Execute(const std::string& host, const USHORT& port,
	                                           const http::request<http::dynamic_body>& request);

private:
	tcp::resolver resolver_;
	beast::tcp_stream http_;
	beast::ssl_stream<beast::tcp_stream> https_;

	bool isSecure_;
};
