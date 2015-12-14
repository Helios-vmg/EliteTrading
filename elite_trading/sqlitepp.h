#ifdef _MSC_VER
#pragma once
#endif

#ifndef SQLITEPP_H
#define SQLITEPP_H
#include <string>
#include <vector>
#include "sqlite3.h"
#include <boost/optional.hpp>

namespace sqlite3pp{
	class Statement;
	class DB{
		sqlite3 *db;
		unsigned lock_count;
	public:
		DB(const char *path,bool Throw=1);
		~DB(){
			if (this->good())
				sqlite3_close(this->db);
		}
		bool good() const{
			return !!this->db;
		}
		operator sqlite3 *() const{
			return this->db;
		}
		Statement operator<<(const char *s);
		void exec(const char *s);
		void begin_transaction(){
			if (!this->lock_count)
				this->exec("begin exclusive transaction;");
			this->lock_count++;
		}
		void commit(){
			if (this->lock_count)
				this->lock_count--;
			if (!this->lock_count)
				this->exec("commit;");
		}
		void rollback(){
			if (this->lock_count){
				this->exec("rollback;");
				this->lock_count=0;
			}
		}
	};
	class Transaction{
		DB *db;
		Transaction(const Transaction &m){}
		void operator=(const Transaction &){}
		bool commit;
	public:
		Transaction(DB &db):db(&db),commit(1){
			db.begin_transaction();
		}
		~Transaction(){
			if (this->commit)
				this->db->commit();
		}
		void rollback(){
			this->db->rollback();
			this->commit=0;
		}
	};
	class Null{};
	class Step{};
	class Reset{};
	class Statement{
		sqlite3 *db;
		sqlite3_stmt *statement;
		unsigned bind_index,
			get_index;
		friend class DB;
		Statement(sqlite3 *db,const char *s);
		void uninit(bool throws=1);
	public:
		Statement():statement(0){}
		Statement(Statement &);
		~Statement();
		Statement &operator=(Statement &);
		bool good() const{
			return !!this->statement;
		}
		bool operator!() const{
			return !this->good();
		}
		void reset();
		Statement &operator<<(const Null &);
		Statement &operator<<(const Step &);
		Statement &operator<<(const Reset &);
		Statement &operator<<(bool b){
			return (*this)<<(int)b;
		}
		Statement &operator<<(int);
		Statement &operator<<(unsigned u){
			return *this<<(int)u;
		}
		Statement &operator<<(sqlite3_int64);
		Statement &operator<<(sqlite3_uint64);
		Statement &operator<<(double);
		Statement &operator<<(const char *s){
			return *this<<std::string(s);
		}
		Statement &operator<<(const std::string &);
		Statement &operator<<(const std::vector<unsigned char> &);
		/*
			How to use:
			while (stmt.step()==SQLITE_ROW){
				//Get more data.
			}
		*/
		int step();
		void set_get(unsigned i){
			this->get_index=i;
		}
		Statement &operator>>(bool &b){
			int i;
			*this >>i;
			b=!!i;
			return *this;
		}
		Statement &operator>>(int &i);
		Statement &operator>>(unsigned &u){
			int i;
			*this>>i;
			u=(unsigned)i;
			return *this;
		}
		Statement &operator>>(sqlite3_int64 &i);
		Statement &operator>>(sqlite3_uint64 &i);
		Statement &operator>>(double &d);
		Statement &operator>>(std::string &s);
		Statement &operator>>(std::vector<unsigned char> &v);
		template <typename T>
		Statement &operator>>(boost::optional<T> &dst){
			if (sqlite3_column_type(this->statement, this->get_index) != SQLITE_NULL){
				T temp;
				*this >> temp;
				dst.emplace(temp);
			}else
				this->get_index++;
			return *this;
		}
		template <typename T>
		Statement &operator<<(boost::optional<T> &src){
			if (!src.is_initialized())
				*this << Null();
			else
				*this << src.value();
			return *this;
		}
	};
	class exception:public std::exception{
		std::string error_str;
	public:
		exception(const std::string &s):error_str(s){}
		virtual ~exception(){}
		const char *what() const{
			return this->error_str.c_str();
		}
	};
	class syntax_error:public exception{
	public:
		syntax_error(const std::string &s):exception(s){}
		~syntax_error(){}
	};
	class argument_error:public exception{
	public:
		argument_error(const std::string &s):exception(s){}
		~argument_error(){}
	};
}
#endif
