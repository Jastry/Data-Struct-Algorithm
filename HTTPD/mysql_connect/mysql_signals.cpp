#include "mysql_signals.h"

/*--------------------------- mysql_signals.h  ---------------------------*/
static sigset_t sigs[1];
static int sigs_inited;

static void init_rts_sigset()
{
	static pthread_mutex_t sigs_mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&sigs_mutex);
	if (!sigs_inited){
		sigemptyset(sigs);
		sigaddset(sigs, SIGALRM);
		sigaddset(sigs, SIGVTALRM);
		sigs_inited = 1;
	}
	pthread_mutex_unlock(&sigs_mutex);
}

#define block_rts_signals() \
	do{ \
		if (!sigs_inited) init_rts_sigset(); \
		pthread_sigmask(SIG_BLOCK, sigs, NULL); \
	} while (0)

#define unblock_rts_signals(); \
	pthread_sigmask(SIG_UNBLOCK, sigs, NULL)


MYSQL* _blb_mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag){
	MYSQL* ret;
	block_rts_signals();
	ret = mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag);
	unblock_rts_signals();
	return ret;	
}

void _blb_mysql_close(MYSQL* mysql)
{
	block_rts_signals();
	mysql_close(mysql);
	unblock_rts_signals();
} 

int _blb_mysql_query(MYSQL* mysql, const char* stmt_str)
{
	int ret;
	block_rts_signals();
	ret = mysql_query( mysql, stmt_str)
	unblock_rts_signals();
	return ret;
}

int _blb_mysql_select(MYSQL* mysql, const char* db)
{
	int ret;
	block_rts_signals();
	ret = mysql_select_db(mysql, db);
	unblock_rts_signals();
	return ret;
}

MYSQL_FIELD* _blb_mysql_fetch_field(MYSQL_RES* result)
{
	MYSQL_FIELD* ret;
	block_rts_signals();
	ret = mysql_fetch_field(result);
	unblock_rts_signals();
	return ret;
}

MYSQL_ROW _blb_mysql_fetch_row(MYSQL_RES* result)
{
	MYSQL_ROW ret;
	block_rts_signals();
	ret = mysql_fetch_row(result);
	unblock_rts_signals();
	return ret;
}

unsigned long* _blb_mysql_fetch_lengths(MYSQL_RES* result)
{
	unsigned long* ret;
	block_rts_signals();
	ret = mysql_fetch_lengths(result);
	unblock_rts_signals();
	return ret;
}

MYSQL_RES* _blb_mysql_use_result(MYSQL* result)
{
	MYSQL_RES* ret;
	block_rts_signals();
	ret = mysql_use_result(result);
	unblock_rts_signals();
	return ret;	
}

MYSQL_RES* _blb_mysql_store_result(MYSQL* mysql)
{
	MYSQL_RES* ret;
	block_rts_signals();
	ret = mysql_store_result(mysql);
	unblock_rts_signals();
	return ret;
}
void _blb_mysql_free_result(MYSQL_RES* mysql)
{
	block_rts_signals();
	mysql_free_result(mysql);
	unblock_rts_signals();
}

void _echo_error(MYSQL* mysql)
{
	printf("Error %u : %s\n", mysql_errno(mysql), mysql_error(mysql));
}

/*-------------     mysql_api.h    ----------------------*/


int mysql_api::select(const char* stmt_str){
	MYSQL_ROW row;
	MYSQL_FIELD* field;
	_blb_mysql_query(_mysql, stmt_str);
	_result = _blb_mysql_store_result(_mysql);
	if (!_result){
		_echo_error(_mysql);
	}
	int fields = mysql_num_fields(_result);
	int i = 0;
	while ( (row = _blb_mysql_fetch_row(_result) )){
		for (i = 0; i < fields; ++i){
			if (0 == i){
				while (field = _blb_mysql_fetch_field(_result)){
					printf("%s ", field->name);
				}
				printf("\n\r");
			}
			printf("%s ", row[i] ? row[i] : "NULL");
		}
	}
	printf("\n\r");
	_blb_mysql_free_result(_result);
	return 0;
}


int mysql_api::connect(){
		_blb_mysql_real_connect(_mysql, _host.c_str(), _user.c_str(), _passwd.c_str(), _db.c_str(), _port, NULL, 0);
		return 0;
}


int mysql_api::create(const char* stmt_str){
	_blb_mysql_query(_mysql, stmt_str);
	return 0;
}

int mysql_api::insert(const char* stmt_str) 
{	
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

/*------------------  process.h    -----------------------------*/

char query[BSIZE];
map<string, string> mp;

void process(const info* msg)
{
	assert(msg);
	int info_len = msg->_len;
	
	int i = 0;
	int j = 0;
	int k = 0;
	char name[SIZE];
	char value[SIZE];
	bzero(name, sizeof (name));
	bzero(value, sizeof (value));
	
	int flag = 1;
	for (; i < info_len; ++i){
		if (msg->_msg[i] == '&'){
			mp[name] = value;			
			//value[0] != '\0' ? value : 'NULL';
			bzero(name, sizeof (name));
			bzero(value, sizeof(value));
			j = k = 0;
			flag = 1;
		}else if (msg->_msg[i] == '='){
			flag--;
		}else if (flag == 1){
			name[j++] = msg->_msg[i];
		}else if (flag == 0){
			value[k++] = msg->_msg[i];
		}
	}
	mp[name] = value;
	map<string, string>::iterator it = mp.begin();
	//for (; it != mp.end(); ++it){
	//	cout << it->first << " : " << it->second << endl;
	//}
	
	//mp["table"] = "PersonInfo";
	mysql_api mysql;
	mysql.connect();
	//p_drop(mysql);
	//p_create(mysql);
	p_insert(mysql);
	p_select(mysql);
}

void p_select(mysql_api& mysql)
{
	const char* str = "SELECT * FROM ";
	bzero(query, sizeof (query));
	snprintf(query, sizeof (query), "%s %s", str, mp["select_table"].c_str());
	mysql.select(query);
	//echo_err
}

void p_create(mysql_api& mysql)
{
	const char* str = "CREATE TABLE ";
	bzero(query, sizeof (query));
	snprintf(query, sizeof (query), "%s %s (name varchar(10), sex varchar(5))charset utf8", str, mp["new_table"].c_str());
	mysql.create(query);
	//echo_err
}

void p_drop(mysql_api& mysql)
{
	const char* str = "DROP TABLE ";
	bzero(query, sizeof (query));
	snprintf(query, sizeof (query), "%s %s", str, mp["drop_table"].c_str());
	mysql.drop(query);
}

void p_insert(mysql_api& mysql)
{
	const char* str = "INSERT INTO ";
	bzero(query, sizeof (query));
	snprintf(query, sizeof (query), "%s %s (name, sex) values ('%s', '%s')",str,mp["table"].c_str(), mp["key_1"].c_str(), mp["key_2"].c_str());
	mysql.insert(query);
}
