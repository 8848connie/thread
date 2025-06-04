/*===============================================
*   文件名称：func.c
*   创 建 者：     
*   创建日期：2025年05月29日
*   描    述：
================================================*/
#include <stdio.h>
#include "func.h"
#include <sqlite3.h>

//界面美化
void print_welcome()
{
    printf("**************************************\n");
    printf("*                                    *\n");
    printf("*                                    *\n");
    printf("*          欢迎使用点餐系统          *\n");
    printf("*                                    *\n");
    printf("*                                    *\n");
    printf("**************************************\n");
}   


//清屏函数
void clear_screen() {

    printf("\033[H\033[J");
}


//库的初始化，返回值为数据库的指针，需要创建一个数据库的指针来得到返回值
sqlite3 *sqlite_init()
{
	//创建数据库
	sqlite3 *db = NULL;
	int err;
	if (0 != (err = sqlite3_open("sail.db", &db))) {
		fprintf(stderr, "open database: %s\n", strerror(err));
		return NULL;
	}
	return db;
}


//商家数据表初始化
sqlite3 *sqlite_sail_table_init(sqlite3 *db)
{
	//创建商家信息表，店名，商家id和商家密码
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//创建商家信息表
	const char *sql = "CREATE TABLE IF NOT EXISTS 商家账号 ("
		"账号 PRIMARY KEY AUTOINCREMENT, "
		"餐厅名 TEXT NOT NULL, "
		"密码 TEXT NOT NULL);";
	char *err_msg = NULL;
	int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return NULL;
	}
	return db;
}


//打开数据库
sqlite3 *sqlite_open(const char *filename)
{
	sqlite3 *db = NULL;
	int err;
	if (0 != (err = sqlite3_open(filename, &db))) {
		fprintf(stderr, "open database: %s\n", strerror(err));
		return NULL;
	}
	return db;
}


//注册商家账号函数，往商家表中插入数据，商家id，商家店名，商家密码
sqlite3 *sqlite_sail_table_insert(sqlite3 *db,const char *id,const char *name, const char *password)
{
	
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//插入商家信息,即商家账号，商店名，密码
	const char *sql = "INSERT INTO 商家账号 (账号, 餐厅名, 密码) VALUES (?, ?, ?);";
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	sqlite3_bind_text(stmt, 1, id, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, password, -1, SQLITE_STATIC);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Failed to insert data: %s\n", sqlite3_errmsg(db));
	}
	sqlite3_finalize(stmt);
	return db;
}


//注册商家菜单，表内有商家店名，商家id，菜名，菜价格，表名为商家id加商家店名
sqlite3 *sqlite_menu_inite(sqlite3 *db, const char *id, const char *name)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//创建商家菜单表
	char sql[256];
	snprintf(sql, sizeof(sql), "CREATE TABLE IF NOT EXISTS %s_%s ("
		"菜名 TEXT NOT NULL, "
		"价格 REAL NOT NULL);", id, name);
	char *err_msg = NULL;
	int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return NULL;
	}
	return db;
}

//在对应的商家菜单表中插入菜名和价格
sqlite3 *sqlite_menu_insert(sqlite3 *db, const char *id, const char *name, const char *food_name, double price)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//插入菜名和价格
	char sql[256];
	snprintf(sql, sizeof(sql), "INSERT INTO %s_%s (菜名, 价格) VALUES (?, ?);", id, name);
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	sqlite3_bind_text(stmt, 1, food_name, -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 2, price);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Failed to insert data: %s\n", sqlite3_errmsg(db));
	}
	sqlite3_finalize(stmt);
	return db;
}



//顾客账号信息初始化函数
sqlite3 *sqlite_customer_table_init(sqlite3 *db)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//创建顾客信息表
	const char *sql = "CREATE TABLE IF NOT EXISTS 顾客账号 ("
		"账号 PRIMARY KEY AUTOINCREMENT, "
		"姓名 TEXT NOT NULL, "
		"密码 TEXT NOT NULL);";
	char *err_msg = NULL;
	int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return NULL;
	}
	return db;
}

//顾客注册账号函数，往顾客表中插入数据，顾客账号，顾客姓名，顾客密码
sqlite3 *sqlite_customer_table_insert(sqlite3 *db, const char *id, const char *name, const char *password)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//插入顾客信息
	const char *sql = "INSERT INTO 顾客账号 (账号, 姓名, 密码) VALUES (?, ?, ?);";
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	sqlite3_bind_text(stmt, 1, id, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, password, -1, SQLITE_STATIC);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Failed to insert data: %s\n", sqlite3_errmsg(db));
	}
	sqlite3_finalize(stmt);
	return db;
}


//顾客查看商家信息，只显示商家店名
sqlite3 *sqlite_view_sail(sqlite3 *db)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//查询商家信息
	const char *sql = "SELECT 餐厅名 FROM 商家账号;";
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		const unsigned char *restaurant_name = sqlite3_column_text(stmt, 0);
		printf("餐厅名: %s\n", restaurant_name);
	}
	sqlite3_finalize(stmt);
	return db;
}

//顾客查看具体商家的菜单表
sqlite3 *sqlite_view_menu(sqlite3 *db, const char *id, const char *name)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//查询商家菜单
	char sql[256];
	snprintf(sql, sizeof(sql), "SELECT * FROM %s_%s;", id, name);
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		const unsigned char *food_name = sqlite3_column_text(stmt, 0);
		double price = sqlite3_column_double(stmt, 1);
		printf("菜名: %s, 价格: %.2f\n", food_name, price);
	}
	sqlite3_finalize(stmt);
	return db;
}


//顾客点单表，表内包含商家店名,商家id,顾客姓名,菜名,菜价格，菜的数量，同一种菜的总价
sqlite3 *sqlite_order_table_init(sqlite3 *db, const char *id, const char *name)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//创建顾客点单表
	char sql[256];
	snprintf(sql, sizeof(sql), "CREATE TABLE IF NOT EXISTS %s_%s_订单 ("
		"商家店名 TEXT NOT NULL, "
		"商家ID TEXT NOT NULL, "
		"顾客姓名 TEXT NOT NULL, "
		"菜名 TEXT NOT NULL, "
		"价格 REAL NOT NULL, "
		"数量 INTEGER NOT NULL);", id, name);
	char *err_msg = NULL;
	int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return NULL;
	}
	return db;
}

//商家查看顾客订单表格，并算出总价格表，返回总价格
sqlite3 *sqlite_view_order(sqlite3 *db, const char *id, const char *name)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//查询顾客订单
	char sql[256];
	snprintf(sql, sizeof(sql), "SELECT * FROM %s_%s_订单;", id, name);
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	double total_price = 0.0;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		const unsigned char *restaurant_name = sqlite3_column_text(stmt, 0);
		const unsigned char *customer_name = sqlite3_column_text(stmt, 2);
		const unsigned char *food_name = sqlite3_column_text(stmt, 3);
		double price = sqlite3_column_double(stmt, 4);
		int quantity = sqlite3_column_int(stmt, 5);
		double total = price * quantity;
		total_price += total;
		printf("商家店名: %s, 顾客姓名: %s, 菜名: %s, 价格: %.2f, 数量: %d, 总价: %.2f\n",
			restaurant_name, customer_name, food_name, price, quantity, total);
	}
	printf("总价格: %.2f\n", total_price);
	sqlite3_finalize(stmt);
	return db;
}

//销毁表
sqlite3 *sqlite_drop_table(sqlite3 *db, const char *table_name)
{
	if (db == NULL) {
		fprintf(stderr, "Database not initialized.\n");
		return NULL;
	}
	//删除表
	char sql[256];
	snprintf(sql, sizeof(sql), "DROP TABLE IF EXISTS %s;", table_name);
	char *err_msg = NULL;
	int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return NULL;
	}
	return db;
}


//
handle_client(void *arg) {
	int client_fd = *(int *)arg;
	free(arg); // 释放分配的内存

	// 在这里处理客户端请求
	// 例如，读取客户端发送的数据，处理请求等

	// 关闭客户端套接字
	close(client_fd);
	return NULL;
}
