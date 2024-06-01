#pragma once
#include "HttpClient.hpp"

class HttpHandler : public boost::enable_shared_from_this<HttpHandler>, boost::noncopyable
{
public:
	explicit HttpHandler(boost::asio::io_service& ioService, const std::shared_ptr<HttpClient>& http);

	using pointer = boost::shared_ptr<HttpHandler>;

	boost::asio::ip::tcp::socket& GameSocket();

	void Start();

private:
	boost::asio::ip::tcp::socket game_socket_;
	std::shared_ptr<HttpClient> http_;

	// The buffer for performing reads.
	beast::flat_buffer buffer_{PACKET_MAX_LENGTH};

	// The request message.
	http::request<http::dynamic_body> request_;
	http::request<http::dynamic_body> requestCopy_;

	// The response message.
	http::response<http::dynamic_body> response_;

	void ProcessRequest(const boost::system::error_code& error, size_t bytesTransferred);
	void HandleWrite(const boost::system::error_code& error, size_t bytesTransferred) const;
};
