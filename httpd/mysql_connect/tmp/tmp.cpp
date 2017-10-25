#include "mysql_api.h"

void mysql_api::version()
{
	cout << "current version is "<< mysql_get_client_info() << endl;
}

int mysql_api::connect(){
		_blb_mysql_real_connect(_conn, _host.c_str(), _user.c_str(), _passwd.c_str(), _db.c_str(), _port, NULL, 0);
		return 0;
}

int mysql_api::create(const string& table_name, const vector<string>& key, const vector<string>& value){
	_blb_mysql_create(table_name, key, value);
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
	if ( !mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag) ){
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
	_blb_mysql_query(_conn, "create table test(NAME varchar(10), sex varchar(5))charset utf8");
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
	_blb_mysql_query(_conn, "insert into PersonInfo(Name, age, sex, ID, tel) values ('test', 21, 'man', '1234567', '12334235')");
	return 0;
}

int mysql_api::_blb_mysql_drop(const char* table_name)
{
		string drop = "drop table ";
		drop += table_name;
		_blb_mysql_query(_conn, drop.c_str());
		return 0;
}

int mysql_api::drop(const char* table)
{
	_blb_mysql_drop(table);
	return 0;
}

int mysql_api::del(const char* table, const char* key, const char* value){
	_blb_mysql_delete(table, key, value);
	return 0;
}

int mysql_api::_blb_mysql_delete(const char* table, const char* key, const char* value)
{
	string str="";
	/**/
	_blb_mysql_query(_conn, "delete from PersonInfo where Name='test'");
	return 0;
}

int mysql_api::modify(const char* table, const char* key, const char* type, const char* pos){
	_blb_mysql_modify(table, key, type, pos);
	return 0;
}
int mysql_api::_blb_mysql_modify(const char* table, const char* key, const char* type, const char* pos){
	_blb_mysql_query(_conn, "alter table PersonInfo modify Name varchar(5) first");
	return 0;
}

int mysql_api::change(const char* table, const char* key, const char* new_name, const char* type, const char* pos){
	_blb_mysql_change(table, key, new_name, type, pos);
	return 0;
}

int mysql_api::_blb_mysql_change(const char* table, const char* key, const char* new_name, const char* type, const char* pos){
	string str = "";
	_blb_mysql_query(_conn, "alter table PersonInfo change sex Sex varchar(7) after Name");
}

int mysql_api::select(const char* table, const char* stmt_str)
{
	_blb_mysql_select(table, stmt_str);
	return 0;
}

int mysql_api::_blb_mysql_select(const char* table, const char* stmt_str){
	
	MYSQL_ROW row;
	MYSQL_FIELD* field;
	_blb_mysql_query(_conn, "select * from PersonInfo");
	_res = mysql_store_result(_conn);
	if (!_res){
		_echo_error(_conn);
	}
	int fields = mysql_num_fields(_res);
	int i = 0;
	while ( (row = mysql_fetch_row(_res) )){
		for (i = 0; i < fields; ++i){
			if (0 == i){
				while (field = mysql_fetch_field(_res)){
					printf("%s ", field->name);
				}
				printf("\n");
			}
			printf("%s ", row[i] ? row[i] : "NULL");
		}
	}
	printf("\n");
	mysql_free_result(_res);
	return 0;
}
