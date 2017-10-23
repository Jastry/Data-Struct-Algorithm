#include "mysql_api.h"

void mysql_api::version()
{
	cout << "current version is "<< mysql_get_client_info() << endl;
}

int mysql_api::connect(){
		//_blb_mysql_real_connect(_conn, _host.c_str(), _user.c_str(), _passwd.c_str(), _db.c_str(), _port, NULL, 0);
		return 0;
}

int mysql_api::_blb_mysql_real_connect(MYSQL *mysql, \
									   const char *host, \
									   const char *user, \
									   const char *passwd, \
									   const char *db, \
									   unsigned int port, \
									   const char *unix_socket, \
									   unsigned long client_flag){
	if ( mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag) ){
		_echo_error(mysql);
		}
	return 0;	
	}

void mysql_api::_echo_error(MYSQL* mysql)
{
	printf("Error %u : %s\n", mysql_errno(mysql), mysql_error(mysql));
}

int mysql_api::_blb_mysql_create(const string& table_name, \
					      		 const vector<string>& key, \
						  		 const vector<string>& values)
{
	/*处理字符串*/
	key.size();
	values.size();
	_blb_mysql_query(_conn, "create table test(varchar(10), sex varchar(5))charset utf8");
	return 0;
}

int mysql_api::_blb_mysql_query(MYSQL* mysql, const char* stmt_str)
{
	if (mysql_query( mysql, stmt_str) ){
		_echo_error(mysql);
	}
	return 0;

}


int mysql_api::insert(const string& table_name, \
					  const vector<string>& key,\
					  const vector<string>& value) {	
	_blb_mysql_insert(table_name, key, value);	
	return 0;
}

int mysql_api::_blb_mysql_insert(const string& table_name, \
								 const vector<string>& key,\
								 const vector<string>& value){
		/* 对字符串修改 */
	key.size();
	_blb_mysql_query(_conn, "insert into PersonInfo(Name, age, sex, ID, tel) values ('test', 21, '男', '1234567', '12334235')");
	return 0;
}



#if 0
#endif
