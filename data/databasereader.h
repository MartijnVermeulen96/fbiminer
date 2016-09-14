#ifndef DATA_DATABASEREADER_H_
#define DATA_DATABASEREADER_H_

#include "database.h"

class DatabaseReader : private Database {
public:
	static Database fromTable(std::ifstream&, char delim=' ', bool headers=true);
	static Database fromItemsets(std::ifstream&, char delim=' ');
};

#endif // DATA_DATABASEREADER_H_