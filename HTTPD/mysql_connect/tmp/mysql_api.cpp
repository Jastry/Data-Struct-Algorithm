#include "mysql_api.h"

int mysql_api::select(const char* stmt_str){
	MYSQL_ROW row;
	MYSQL_FIELD* field;
	_blb_mysql_query(_mysql, stmt_str);
	_res = _blb_mysql_store_result(_mysql);
	if (!_res){
		_echo_error(_mysql);
	}
	int fields = mysql_num_fields(_res);
	int i = 0;
	while ( (row = _blb_mysql_fetch_row(_res) )){
		for (i = 0; i < fields; ++i){
			if (0 == i){
				while (field = _blb_mysql_fetch_field(_res)){
					printf("%s ", field->name);
				}
				printf("\n");
			}
			printf("%s ", row[i] ? row[i] : "NULL");
		}
	}
	printf("\n");
	_blb_mysql_free_result(_res);
	return 0;
}

/*
int mysql_api::connect(){
		_blb_mysql_real_connect(_mysql, _host.c_str(), _user.c_str(), _passwd.c_str(), _db.c_str(), _port, NULL, 0);
		return 0;
}

int mysql_api::create(const char* stmt_str){
	_blb_mysql_query(stmt_str);
	return 0;
}

void mysql_api::_echo_error(MYSQL* mysql)
{
	printf("Error %u : %s\n", mysql_errno(mysql), mysql_error(mysql));
}

int mysql_api::_blb_mysql_create(const char* stmt_str)
{
	_blb_mysql_query(_mysql, stmt_str);
	return 0;
}

int mysql_api::insert(const char* stmt_str) 
{	
	_blb_mysql_query(stmt_str);	
	return 0;
}

int mysql_api::_blb_mysql_insert(const char* stmt_str){
	_blb_mysql_query(_mysql, stmt_str);
	return 0;
}

int mysql_api::drop(const char* stmt_str)
{
	_blb_mysql_query(_mysql, stmt_str);
	return 0;
}

int mysql_api::del(const char* stmt_str)
{
	_blb_mysql_query(_mysql, stmt_str);
	return 0;
}

int mysql_api::modify(const char* stmt_str)
{
	_blb_mysql_query(_mysql, stmt_str);
	return 0;
}


int mysql_api::change(const char* stmt_str)
{
	_blb_mysql_query(_mysql, stmt_str);
	return 0;
}

void mysql_api::version()
{
	cout << "current version is "<< mysql_get_client_info() << endl;
}
*/
