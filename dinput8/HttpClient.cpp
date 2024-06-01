#include "pch.hpp"
#include "HttpClient.hpp"

HttpClient::HttpClient(net::io_context& ioc, ssl::context& ctx, const bool isSecure) : resolver_(ioc), http_(ioc),
	https_(ioc, ctx)
{
	isSecure_ = isSecure;
}

http::response<http::dynamic_body> HttpClient::Execute(const std::string& host, const USHORT& port,
                                                       const http::request<http::dynamic_body>& request)
{
	// Look up the domain name
	const auto results = resolver_.resolve(host, std::to_string(port));

	// This buffer is used for reading and must be persisted
	beast::flat_buffer buffer;

	// Declare a container to hold the response
	http::response<http::dynamic_body> res;

	beast::error_code ec;

	if (isSecure_)
	{
		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(https_.native_handle(), host.c_str()))
			throw beast::system_error(
				beast::error_code(static_cast<int>(ERR_get_error()), net::error::get_ssl_category()),
				"Failed to set SNI Hostname");

		// Make the connection on the IP address we get from a lookup
		get_lowest_layer(https_).connect(results);

		// Perform the SSL handshake
		https_.handshake(ssl::stream_base::client);

		// Send the HTTP request to the remote host
		http::write(https_, request);

		// Receive the HTTP response
		http::read(https_, buffer, res);

		// Gracefully close the stream
		https_.shutdown(ec);

		if (ec == net::error::eof)
		{
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			ec = {};
		}

		if (ec)
			throw beast::system_error{ec};
	}
	else
	{
		// Make the connection on the IP address we get from a lookup
		http_.connect(results);

		// Send the HTTP request to the remote host
		http::write(http_, request);

		// Receive the HTTP response
		http::read(http_, buffer, res);

		http_.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ec};
	}

	return res;
}
