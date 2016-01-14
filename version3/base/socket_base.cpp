#include "stdafx.h"
#include "socket_base.hpp"

socket_core::socket_core(boost::asio::io_service & io, protocal::protocal_type t, bool need_extern)
{
	type = t;
	switch (t)
	{
	case protocal::TCP:
		make_shared(tcp_opt, io);
		break;
	case protocal::TCP_SSL:
		make_shared(tcp_ssl_opt, io, boost::asio::ssl::context(boost::asio::ssl::context::sslv23_client));
		break;
	default:
		assert(false);
		break;
	}
	if (need_extern)
	{
		make_shared(extern_info);
	}
}

socket_core::socket_core(boost::asio::io_service & io, boost::asio::ssl::context & context, bool need_extern)
{
	type = protocal::TCP_SSL;
	make_shared(tcp_ssl_opt, io, context);
	if (need_extern)
	{
		make_shared(extern_info);
	}
}

boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::stream_socket_service<boost::asio::ip::tcp>> & socket_core::get_socket()
{
	if (type == protocal::TCP_SSL)
		return tcp_ssl_opt->lowest_layer();
	return tcp_opt->lowest_layer();
}

boost::asio::io_service& socket_core::get_io()
{
	return get_socket().get_io_service();
}

void socket_core::cancel(void)
{
	boost::system::error_code ec;
	get_socket().cancel(ec);
}

bool socket_core::verify_certificate(bool preverified, boost::asio::ssl::verify_context & ctx)
{
	// The verify callback can be used to check whether the certificate that is
	// being presented is valid for the peer. For example, RFC 2818 describes
	// the steps involved in doing this for HTTPS. Consult the OpenSSL
	// documentation for more details. Note that the callback is called once
	// for each certificate in the certificate chain, starting from the root
	// certificate authority.

	// In this example we will simply print the certificate's subject name.
#ifdef _DEBUG
	char subject_name[256];
	std::stringstream ss;
	std::string str;
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);

	//std::cout << "Verifying " << subject_name << "\n";

	ss << "name: " << subject_name << "\nversion: " << X509_get_version(cert);
	str = ss.str();

	std::copy(str.begin(), str.end(), std::back_inserter(*extern_info));

	//X509_NAME_oneline(X509_get_issuer_name(cert), subject_name, 256);
	//std::cout << "issuer_name " << subject_name << "\n";

	//auto p = X509_get1_email(cert);
	//auto n = X509_get_serialNumber(cert);
	//auto v = X509_get_version(cert);
	//auto k = X509_get_pubkey(cert);
#endif
	//return preverified;
	return true;
}

http_server_base::http_server_base(boost::asio::io_service & io)
{
	make_shared(accept_opt, io);
}

http_server_base::~http_server_base()
{
}

boost::asio::io_service & http_server_base::get_io()
{
	return accept_opt->get_io_service();
}

void http_server_base::bind_listen(boost::system::error_code & ec, boost::asio::ip::tcp::endpoint ep)
{
	accept_opt->open(ep.protocol(), ec);
	if (ec)
		return;
	accept_opt->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
	if (ec)
		return;
	accept_opt->bind(ep, ec);
	if (ec)
		return;
	accept_opt->listen(boost::asio::socket_base::max_connections, ec);
}

void http_server_base::cancel()
{
	if (accept_opt)
		accept_opt->cancel();
}

std::string http_server_base::get_password(std::size_t max_length, boost::asio::ssl::context_base::password_purpose purpose)
{
	static const std::string both_key("test"); // same as key.pem

	const std::string& key(purpose == boost::asio::ssl::context_base::for_reading ? both_key : both_key);

	if (key.length() < max_length)
		return key;
	else
		return key.substr(0, max_length);
}
