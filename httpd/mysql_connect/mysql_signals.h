
#ifndef __MYSQLSIGNALS_H__
#define __MYSQLSIGNALS_H__

#include <iostream>
#include <map>
#include <unistd.h>
#include <my_global.h>
#include <mysql.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

using namespace std;

MYSQL* _blb_mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);
int  _blb_mysql_query(MYSQL* mysql, const char* stmt_str);
int  _blb_mysql_select_db(MYSQL *mysql, const char *db);
void _echo_error(MYSQL* mysql);
void _blb_mysql_close(MYSQL* mysql);
void _blb_mysql_free_result(MYSQL_RES *result);
MYSQL_FIELD* _blb_mysql_fetch_field(MYSQL* mysql);
MYSQL_ROW _blb_mysql_fetch_row(MYSQL_RES *result);
MYSQL_RES* _blb_mysql_store_result(MYSQL *mysql);	
unsigned long* _blb_mysql_fetch_lengths(MYSQL_RES *result);

#endif //__MYSQLSIGNALS_H__

#ifndef __MYSQLAPI_H__
#define __MYSQLAPI_H__
class mysql_api{
public:
    mysql_api()
    : _host("127.0.0.1")
	, _user("root")
	, _passwd("1")
	, _db("person")
	, _port(3306)
	, _result(NULL)
	{
		_mysql = mysql_init(NULL);
	}

	~mysql_api()
	{
		mysql_close(_mysql);
	}
	void version();
	int connect();
	int create(const char* stmt_str);
	int insert(const char* stmt_str);
	int drop(const char* stmt_str);
	int del(const char* stmt_str);
	int modify(const char* stmt_str);
	int change(const char* stmt_str);
	int select(const char* stmt_str);
private:

protected:
	MYSQL* _mysql;
	MYSQL_RES* _result;
	string _host;
	string _user;
	string _passwd;
	string _db;
	int _port;	
};

#endif //__MYSQLAPI_H__


#ifndef __PROCESS_H__
#define __PROCESS_H__


typedef struct client_info{
	char _method[10];
	char _msg[1024];
	size_t _len;
	client_info(){
		bzero(_method, sizeof (_method));
		bzero(_msg, sizeof (_msg));
		_len = 0;
	}
}info;

void process(const info* msg);
#endif //__PROCESS_H__
