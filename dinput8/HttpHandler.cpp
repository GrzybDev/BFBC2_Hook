#include "pch.hpp"
#include "HttpHandler.hpp"

using namespace boost;
using namespace asio;
using ip::tcp;

HttpHandler::HttpHandler(io_service& ioService, const std::shared_ptr<HttpClient>& http) : game_socket_(ioService)
{
	BOOST_LOG_FUNCTION()

	http_ = http;
}

tcp::socket& HttpHandler::GameSocket()
{
	return game_socket_;
}

void HttpHandler::Start()
{
	async_read(game_socket_, buffer_, request_, bind(&HttpHandler::ProcessRequest, shared_from_this(),
	                                                 asio::placeholders::error,
	                                                 asio::placeholders::bytes_transferred));
}

void HttpHandler::ProcessRequest(const system::error_code& error, size_t bytesTransferred)
{
	if (!error)
	{
		const auto config = &Config::GetInstance();

		// Set up an HTTP GET request message
		request_.set(http::field::host, config->hook->proxyAddress);

		try
		{
			response_ = http_->Execute(config->hook->proxyAddress, config->hook->proxyPort, request_);

			http::async_write(game_socket_, response_, bind(&HttpHandler::HandleWrite, shared_from_this(),
			                                                asio::placeholders::error,
			                                                asio::placeholders::bytes_transferred));
		}
		catch (const system::system_error& e)
		{
			BOOST_LOG_TRIVIAL(error) << format("Failed to handle HTTP request: %s") % e.what();
			game_socket_.close();
		}

		requestCopy_ = request_;

		async_read(game_socket_, buffer_, request_, bind(&HttpHandler::ProcessRequest, shared_from_this(),
		                                                 asio::placeholders::error,
		                                                 asio::placeholders::bytes_transferred));
	}
}

void HttpHandler::HandleWrite(const system::error_code& error, size_t bytesTransferred) const
{
	BOOST_LOG_NAMED_SCOPE("HTTP")

	auto& request = requestCopy_.base();
	BOOST_LOG_TRIVIAL(debug) << format("%s %s [%s %s]") % request.method_string() % request.target() % response_.
result_int() % response_.result();
}
