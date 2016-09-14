#ifndef SRC_UTIL_OUTPUT_H_
#define SRC_UTIL_OUTPUT_H_

#include <iostream>
#include <ostream>
#include "../data/database.h"
#include "../algorithms/forbiddenitemset.h"

class Output {
public:
	template <typename T>
    static void printCollection(const T& coll, std::ostream& out=std::cout) {
		for (const typename T::value_type& i : coll) {
			out << i << " ";
		}
	}

    template<typename T>
    static void printCollection(const T& items, const std::string& join, std::ostream& out=std::cout) {
        bool comma = false;
        for (const typename T::value_type& item : items) {
            if (comma) {
                out << join;
            }
            else {
                comma = true;
            }
            out << item;
        }
    }

    static void printItemset(const ForbiddenItemset& items, const Database& db, std::ostream& out=std::cout, bool endl=true) {
        printItemset(items.getItems(), db, out, endl);
    }

    static void printItemset(const Itemset& items, const Database& db, std::ostream& out=std::cout, bool endl=true) {
        out << "(";
        std::vector<std::string> parts;
        for (uint ix = 0; ix < items.size(); ix++) {
            int item = items[ix];
            if (item < 0) {
                parts.push_back(db.getAttrName(-1-item));
            }
            else {
                parts.push_back(db.getAttrName(db.getAttrIndex(item)) + "=" + db.getValue(item));
            }
        }
        printCollection(parts, ", ", out);
        out << ")";
        if (endl) {
            out << std::endl;
        }
    }
};

#endif /* SRC_UTIL_OUTPUT_H_ */
