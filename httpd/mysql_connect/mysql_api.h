#if 0
//#ifndef __MYSQLAPI_H__
#define __MYSQLAPI_H__

#include <my_global.h>
#include <mysql.h>
#include <string>
#include <vector>

using namespace std;

class mysql_api{
public:
	mysql_api();
	~mysql_api();
	int connect();
	int insert(const string& table_name, const vector<string>& key, const vector<int>& value);
	int create_table(const string& table_name, vector<string> key, vector<string> values);
/*	------------------------------------------------------ */
	int insert();
	int create_table();

private:
	void _echo_error(const MYSQL* conn);
	int  _blb_mysql_query(const MYSQL* conn, const char* stmt_str);
	int  _blb_mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);
	int  _blb_mysql_insert(const string& table_name, const vector<string>& key, const vector<int>& value);
	int  _blb_mysql_create(const string& table_name, const vector<string>& key, const vector<string>& values);

protected:
	MYSQL *_conn;
	MYSQL_RES *_res;
	string _host;
	string _user;
	string _passwd;
	string _db;
	int _port;	
};

#endif //__MYSQLAPI_H__

#ifndef __MYSQLAPI_H__
#define __MYSQLAPI_H__

#include <iostream>
#include <my_global.h>
#include <mysql.h>
#include <string>
#include <vector>

using namespace std;

class mysql_api{
public:
    mysql_api()
    : _host("127.0.0.1")
	, _user("root")
	, _passwd("")
	, _db("person")
	, _port(3306)
	, _res(NULL)
	{
		_conn = mysql_init(NULL);
	}

	~mysql_api()
	{
		mysql_close(_conn);
	}
	void version();
	int connect();
	int insert(const string& table_name, \
			   const vector<string>& key, \
			   const vector<string>& value);
private:
	int _blb_mysql_real_connect(MYSQL *mysql, \
								const char *host, \
								const char *user, \
								const char *passwd, \
								const char *db, \
								unsigned int port, \
								const char *unix_socket, \
								unsigned long client_flag);
	
	int _blb_mysql_insert(const string& table_name, \
						  const vector<string>& key, \
						  const vector<string>& value);
	
	int _blb_mysql_create(const string& table_name, \
					      const vector<string>& key, \
						  const vector<string>& values);
	
	int _blb_mysql_query(MYSQL* mysql, const char* stmt_str);
	
	void _echo_error(MYSQL* mysql);
	

protected:
	MYSQL *_conn;
	MYSQL_RES *_res;
	string _host;
	string _user;
	string _passwd;
	string _db;
	int _port;	
};

#endif
