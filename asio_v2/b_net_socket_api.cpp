#include "stdafx.h"
#include "b_net_socket_api.h"



b_net_socket_api::b_net_socket_api(protocal_type type, boost::shared_ptr<boost::asio::io_service> io, boost::shared_ptr<boost::asio::ssl::context> ctx_opt)
	:s_type(type), io_opt(io), cancel_deadline_func(boost::bind(&b_net_socket_api::cancel_deadline,this))
{
	make_shared(dlt_opt, *io_opt);
	switch (s_type)
	{
	case HTTP:
		make_shared(s_opt.http_opt, *io_opt);
		break;
	case HTTPS:
		if(ctx_opt)
			make_shared(s_opt.https_opt, *io_opt, ctx_opt);
		else
			make_shared(s_opt.https_opt, *io_opt);
		break;
	default:
		assert(false);
		break;
	}
}

b_net_socket_api::~b_net_socket_api()
{
}

boost::shared_ptr<boost::asio::io_service> b_net_socket_api::io()
{
	return io_opt;
}

bool b_net_socket_api::get_socket(boost::shared_ptr<http_base>& ret)
{
	if (s_type != HTTP)
	{
		return false;
	}
	ret = s_opt.http_opt;
	return true;
}

bool b_net_socket_api::get_socket(boost::shared_ptr<https_base>& ret)
{
	if (s_type != HTTPS)
	{
		return false;
	}
	ret = s_opt.https_opt;
	return true;
}

protocal_type b_net_socket_api::get_type()
{
	return s_type;
}

bool b_net_socket_api::set_dlt_time(dlt_type no, size_t delay)
{
	if (no >= dlt_max || no < 0)
		return false;
	if (v_dlt.size() < dlt_max)
	{
		v_dlt.resize(dlt_max, 0);
	}
	v_dlt[no] = delay;
	return true;
}

size_t b_net_socket_api::get_dlt_time(dlt_type no)
{
	if (v_dlt.size() <= no)
		return 0;

	return v_dlt[no];
}

bool b_net_socket_api::connect(boost::asio::yield_context yc, boost::system::error_code & ec, boost::asio::ip::tcp::endpoint ep)
{
	bool ret = false;
	start_deadline(dlt_connect);
	if (ec)
		return ret;
	my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
	switch (s_type)
	{
	case HTTP:
		assert(s_opt.http_opt);
		s_opt.http_opt->connect(yc, ec, ep);
		ret = !ec;
		break;
	case HTTPS:
		assert(s_opt.https_opt);
		s_opt.https_opt->connect(yc, ec, ep);
		ret = !ec;
		break;
	default:
		assert(false);
		break;
	}
	return ret;
}

void b_net_socket_api::cancel(void)
{
	switch (s_type)
	{
	case HTTP:
		assert(s_opt.http_opt);
		return s_opt.http_opt->cancel();
		break;
	case HTTPS:
		assert(s_opt.https_opt);
		return s_opt.https_opt->cancel();
		break;
	default:
		assert(false);
		break;
	}
}

bool b_net_socket_api::start_deadline(dlt_type no)
{
	if (!dlt_opt || !get_dlt_time(no))
		return false;
	boost::system::error_code ec;
	dlt_opt->expires_from_now(boost::posix_time::seconds(get_dlt_time(no)), ec);
	if (ec)
		return false;
	dlt_opt->async_wait(boost::bind(&b_net_socket_api::deadline, shared_from_this(), _1));
	return true;
}

void b_net_socket_api::deadline(boost::system::error_code ec)
{
	if (!ec)
	{
		cancel();
	}
}

void b_net_socket_api::cancel_deadline()
{
	boost::system::error_code ec;
	if (dlt_opt)
		dlt_opt->cancel(ec);
}

bool b_net_socket_api::get_extern_info(std::vector<unsigned char>& ret)
{
	switch (s_type)
	{
	case HTTP:
		assert(s_opt.http_opt);
		return s_opt.http_opt->get_extern_info(ret);
		break;
	case HTTPS:
		assert(s_opt.https_opt);
		return s_opt.https_opt->get_extern_info(ret);
		break;
	default:
		assert(false);
		break;
	}
	return false;
}

protocal_socket::protocal_socket(boost::shared_ptr<http_base> h)
	:http_opt(h)
{
}


protocal_socket::protocal_socket(boost::shared_ptr<https_base> hs)
	: https_opt(hs)
{
}

protocal_socket::protocal_socket()
{
}

