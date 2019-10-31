#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include "data/databasereader.h"
#include "algorithms/fbiminer.h"
#include "algorithms/almostfbiminer.h"
#include "algorithms/repairer.h"
#include "util/output.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: ./Cleaner infile tau" << std::endl;
		std::cout << "\t where infile is a .csv dataset with headers, and tau a value in ]0, 0.75[" << std::endl;
    }
    else {
		// parse arguments
        auto t1 = std::chrono::high_resolution_clock::now();
        std::ifstream ifile(argv[1]);
        Database db = DatabaseReader::fromTable(ifile, ',');
        double tau = atof(argv[2]);
		
		// discover FBIs
        FBIMiner m(db, tau);
        m.run();
        std::cout << "Forbidden Itemsets:" << std::endl;
        for (const ForbiddenItemset& fm : m.getRules()) {
            Output::printItemset(fm, db);
        }
		
		// find erroneous objects
        std::vector<int> errorTids;
        for (unsigned i = 0; i < db.size(); i++) {
            const Transaction& tup = db.getRow(i);
            for (const ForbiddenItemset& r : m.getRules()) {
                if (r.match(tup)) {
                    errorTids.push_back(i);
                    break;
                }
            }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto etime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << std::endl << "Elapsed time: " << etime << std::endl;
		
        std::cout << "--------------" << std::endl << std::endl;
        std::cout << "Repairs:" << std::endl;
		
		// discover almost fbis to guarantee clean repair
		// commenting out the am.run() line should disable a-fbi checking
        AlmostFBIMiner am(db, tau, errorTids);
        am.run();
		
		// perform repairs for each dirty object
        Repairer valueCl(db, m.getRules());
        valueCl.setAlmost(am.getRules(), tau);
        for (int tid : errorTids) {
            std::cout << "Dirty Object: ";
            Output::printItemset(db.getRow(tid), db);
            std::cout << "Repaired Transaction: ";
            Output::printItemset(valueCl.getRepair(tid), db);  
            std::cout << std::endl << "--------------" << std::endl;
        }
    }
}
