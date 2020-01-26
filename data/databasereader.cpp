#include "databasereader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Database DatabaseReader::fromItemsets(std::ifstream& infile, char delim) {
	DatabaseReader db = DatabaseReader();
	std::vector<std::string> heads;
	heads.push_back("Item");
    db.setAttributes(heads);

	std::string line;
	std::getline(infile, line);
	bool eof = false;
	while (!eof)
	{
	    std::istringstream iss(line);
	    std::string val;
        std::vector<int> row;
    	int i = 0;
	    while (std::getline(iss, val, delim)) {
            row.push_back(db.translateToken(1,val));
    		i++;
	    }
	    if (i > 0) {
	    	db.addRow(row);
	    }
	    if (infile.eof()) {
	    	eof = true;
	    }
    	std::getline(infile, line);
	}
	return db;
}


Database DatabaseReader::fromTable(std::ifstream& infile, char delim, bool headers) {
	DatabaseReader db = DatabaseReader();
	std::string line;
	std::vector<std::string> heads;
	std::getline(infile, line);
	std::istringstream iss(line);
	std::string val;
	int i = 1;
	while (std::getline(iss, val, delim)) {
		if (headers)
			heads.push_back(val);
		else
            heads.push_back("Attr" + std::to_string(i));
		i++;
	}
	db.setAttributes(heads);
	if (headers)
		std::getline(infile, line);
	int size = i - 1;
	bool eof = false;
	int j = 0;
	while (!eof)
	{
		j++;
	    std::istringstream iss(line);
	    std::string val;
        std::vector<int> row(size);
    	int i = 0;
		bool readerror = false;

	    while (std::getline(iss, val, delim)) {
			if(val[0] == '"'){
				val.erase(0, 1);
				std::string tempval;
				while(std::getline(iss, tempval, delim)){
					val += "," + tempval;
					if(tempval[tempval.length() - 1] == '"'){
						val.erase(val.length() - 1);
						break;
					}
				}
			}
			if(i >= size){
				readerror = true;
				
				// break;
			}
	    	int it = db.translateToken(i, val);
	    	db.incFreq(it);
            row[i] = it;
    		i++;
	    }

		if(!readerror && i > 0){
            db.addRow(row);
	    }
	    if (infile.eof()) {
	    	eof = true;
	    }
    	std::getline(infile, line);

	}
	return db;
}