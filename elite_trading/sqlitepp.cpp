#include "sqlite3.h"
#include "sqlitepp.h"

namespace sqlite3pp{
	static void throw_sqlite_error(int error,sqlite3 *db){
		if (error==SQLITE_OK || error==SQLITE_DONE || error==SQLITE_ROW)
			return;
		std::string error_msg=sqlite3_errmsg(db);
		switch (error){
			case SQLITE_ERROR:
				throw exception(std::string("Generic SQLite error: ")+error_msg);
			case SQLITE_INTERNAL:
				throw exception(std::string("Internal SQLite error: ")+error_msg);
			case SQLITE_PERM:
				throw exception(std::string("Permission denied. Error string: ")+error_msg);
			case SQLITE_ABORT:
				throw exception(std::string("Abort requested. Error string: ")+error_msg);
			case SQLITE_BUSY:
				throw exception(std::string("Database is locked. Error string: ")+error_msg);
			case SQLITE_LOCKED:
				throw exception(std::string("Table is locked. Error string: ")+error_msg);
			case SQLITE_NOMEM:
				throw exception(std::string("malloc() failed. Error string: ")+error_msg);
			case SQLITE_READONLY:
				throw exception(std::string("Database is read-only. Error string: ")+error_msg);
			case SQLITE_INTERRUPT:
				throw exception(std::string("sqlite3_interrupt() called. Error string: ")+error_msg);
			case SQLITE_IOERR:
				throw exception(std::string("I/O error: ")+error_msg);
			case SQLITE_CORRUPT:
				throw exception(std::string("Database is corrupt. Error string: ")+error_msg);
			case SQLITE_NOTFOUND:
				break;
			case SQLITE_FULL:
				throw exception(std::string("Database is full. Error string: ")+error_msg);
			case SQLITE_CANTOPEN:
				throw exception(std::string("Can't open database. Error string: ")+error_msg);
			case SQLITE_PROTOCOL:
				break;
			case SQLITE_EMPTY:
				throw exception(std::string("Database is empty. Error string: ")+error_msg);
			case SQLITE_SCHEMA:
				throw exception(std::string("Database schema changed. Error string: ")+error_msg);
			case SQLITE_TOOBIG:
				throw exception(std::string("String or blob above size limit. Error string: ")+error_msg);
			case SQLITE_CONSTRAINT:
				throw exception(std::string("Constraint violation. Error string: ")+error_msg);
			case SQLITE_MISMATCH:
				throw exception(std::string("Data type mismatch. Error string: ")+error_msg);
			case SQLITE_MISUSE:
				throw exception(std::string("SQLite used incorrectly. Error string: ")+error_msg);
			case SQLITE_NOLFS:
				throw exception(std::string("OS features not supported by host. Error string: ")+error_msg);
			case SQLITE_AUTH:
				throw exception(std::string("Authorization denied. Error string: ")+error_msg);
			case SQLITE_FORMAT:
				throw exception(std::string("Auxiliary database format error. Error string: ")+error_msg);
			case SQLITE_RANGE:
				throw exception(std::string("Range error. Error string: ")+error_msg);
			case SQLITE_NOTADB:
				throw exception(std::string("File is not a database. Error string: ")+error_msg);
		}
	}

	DB::DB(const char *path,bool Throw):lock_count(0){
		int error=sqlite3_open(path,&this->db);
		if (Throw)
			throw_sqlite_error(error,this->db);
		else if (error!=SQLITE_OK){
			sqlite3_close(this->db);
			this->db=0;
		}
	}

	void DB::exec(const char *s){
		if (!this->good())
			return;
		int error=sqlite3_exec(this->db,s,0,0,0);
		throw_sqlite_error(error,this->db);
	}

	Statement DB::operator<<(const char *s){
		return Statement(this->db,s);
	}

	Statement::Statement(sqlite3 *db,const char *s){
		this->db=db;
		int error=sqlite3_prepare_v2(db,s,-1,&this->statement,0);
		throw_sqlite_error(error,db);
		this->reset();
	}

	Statement::Statement(Statement &s){
		this->db=s.db;
		this->statement=s.statement;
		this->reset();
		s.statement=0;
	}

	void Statement::uninit(bool throws){
		if (!this->good())
			return;
		int error=sqlite3_finalize(this->statement);
		if (throws)
			throw_sqlite_error(error,this->db);
	}

	Statement::~Statement(){
		this->uninit(0);
	}

	Statement &Statement::operator=(Statement &s){
		this->uninit();
		this->db=s.db;
		this->statement=s.statement;
		s.statement=0;
		return *this;
	}

	void Statement::reset(){
		if (!*this)
			return;
		this->bind_index=1;
		int error;
		error=sqlite3_reset(this->statement);
		throw_sqlite_error(error,this->db);
		error=sqlite3_clear_bindings(this->statement);
		throw_sqlite_error(error,this->db);
	}

	Statement &Statement::operator<<(const Null &){
		if (!*this)
			return *this;
		int error=sqlite3_bind_null(this->statement,this->bind_index++);
		throw_sqlite_error(error,this->db);
		return *this;
	}

	Statement &Statement::operator<<(const Step &){
		if (!*this)
			return *this;
		this->step();
		return *this;
	}

	Statement &Statement::operator<<(const Reset &){
		if (!*this)
			return *this;
		this->reset();
		return *this;
	}

	Statement &Statement::operator<<(int i){
		if (!*this)
			return *this;
		int error=sqlite3_bind_int(this->statement,this->bind_index++,i);
		throw_sqlite_error(error,this->db);
		return *this;
	}

	Statement &Statement::operator<<(sqlite3_int64 i){
		if (!*this)
			return *this;
		int error=sqlite3_bind_int64(this->statement,this->bind_index++,i);
		throw_sqlite_error(error,this->db);
		return *this;
	}

	Statement &Statement::operator<<(sqlite3_uint64 i){
		return *this<<(sqlite3_int64)i;
	}

	Statement &Statement::operator<<(double d){
		if (!*this)
			return *this;
		int error=sqlite3_bind_double(this->statement,this->bind_index++,d);
		throw_sqlite_error(error,this->db);
		return *this;
	}

	Statement &Statement::operator<<(const std::string &s){
		if (!*this)
			return *this;
		int error=sqlite3_bind_text(this->statement,this->bind_index++,s.c_str(),s.size(),SQLITE_TRANSIENT);
		throw_sqlite_error(error,this->db);
		return *this;
	}

	Statement &Statement::operator<<(const std::vector<unsigned char> &v){
		if (!*this)
			return *this;
		int error=sqlite3_bind_blob(this->statement,this->bind_index++,&v[0],v.size(),SQLITE_TRANSIENT);
		throw_sqlite_error(error,this->db);
		return *this;
	}

	int Statement::step(){
		if (!*this)
			return SQLITE_ERROR;
		this->get_index=0;
		int error=sqlite3_step(this->statement);
		throw_sqlite_error(error,this->db);
		return error;
	}

	Statement &Statement::operator>>(int &i){
		i=sqlite3_column_int(this->statement,this->get_index++);
		return *this;
	}

	Statement &Statement::operator>>(sqlite3_int64 &i){
		i=sqlite3_column_int64(this->statement,this->get_index++);
		return *this;
	}

	Statement &Statement::operator>>(sqlite3_uint64 &i){
		sqlite3_int64 temp;
		*this >>temp;
		i=temp;
		return *this;
	}

	Statement &Statement::operator>>(double &d){
		d=sqlite3_column_double(this->statement,this->get_index++);
		return *this;
	}

	Statement &Statement::operator>>(std::string &s){
		sqlite3_column_text(this->statement,this->get_index);
		size_t size=sqlite3_column_bytes(this->statement,this->get_index);
		s.resize(size);
		const char *p=(const char *)sqlite3_column_text(this->statement,this->get_index++);
		std::copy(p,p+size,s.begin());
		return *this;
	}

	Statement &Statement::operator>>(std::vector<unsigned char> &v){
		sqlite3_column_blob(this->statement,this->get_index);
		v.resize(sqlite3_column_bytes(this->statement,this->get_index));
		memcpy(&v[0],sqlite3_column_blob(this->statement,this->get_index),v.size());
		this->get_index++;
		return *this;
	}
}
