#ifndef __MYSQLAPI_H__
#define __MYSQLAPI_H__

#include <iostream>
#include <my_global.h>
#include <mysql.h>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;
const int BUFFSIZE = 100;
const int LITTLEBUFF = 40;

class mysql_api{
public:
    mysql_api()
    : _host("127.0.0.1")
	, _user("root")
	, _passwd("1")
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
	int create(const string& table_name, const vector<string>& key, const vector<string>& value);
	int insert(const string& table_name, \
			   const vector<string>& key, \
			   const vector<string>& value);
	int drop(const char* table);
	int del(const char* table, const char* key, const char* value);
	int modify(const char* table, const char* key, const char* type, const char* pos);
	int change(const char* table, const char* key, const char* new_name, const char* type, const char* stmt_);
	int select(const char* table, const char* stmt_str);
private:
	
	int _blb_mysql_select(const char* table, const char* stmt_str);
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
		
	int _blb_mysql_drop(const char* table_name);
	
	int _blb_mysql_delete(const char* table, const char* key, const char* value);
	int _blb_mysql_modify(const char* table, const char* key, const char* type, const char* pos);
	int _blb_mysql_change(const char* table, const char* key, const char* new_name, const char* type, const char* pos);
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
