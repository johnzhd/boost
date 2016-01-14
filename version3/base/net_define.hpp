#pragma once


namespace protocal
{
	typedef enum protocal_type
	{
		UNKOWN = 0,
		TCP = 1,  // for http_base
		TCP_SSL,     // for https_base

		HTTP = 80,
		HTTPS = 443,
		ZOOKEEPER = 2181,
		MYSQL = 3306,
		REDIS = 6379,

		MSSQL,
		SOLR,
		ELASTICSEARCH = 9200,

		HADOOP = 50070,

		protocal_max
	}protocal_type;
}


