#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include "data/databasereader.h"
#include "algorithms/fbiminer.h"
#include "algorithms/almostfbiminer.h"
#include "algorithms/repairer.h"
#include "util/output.h"

const int START_INDEX = 0;
const int NR_DATASETS = 6;
const char* DATASETS[] = {
    "Adult",
    "CensusIncome",
    "CreditCard",
    "Ipums",
    "LetterRecognition",
    "Mushroom",
};

const double NR_INTERVALS = 10;
const double LIFTS[] = {
    0.01, 0.1,
    0.001, 0.01,
    0.01, 0.1,
    0.001, 0.01,
    0.01, 0.1,
    0.01, 0.1,
};

const int NR_RUNS = 5;

const char* DATA_PATH = "datasets/";

int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cout << "Usage: ./Cleaner infile tau" << std::endl;
    }
    else if (argc == 3) {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::ifstream ifile(argv[1]);
        Database db = DatabaseReader::fromTable(ifile, ',');
        double tau = atof(argv[2]);
        FBIMiner m(db, tau);;
        m.run();
        std::cout << "Forbidden Itemsets:" << std::endl;
        for (const ForbiddenItemset& fm : m.getRules()) {
            Output::printItemset(fm, db);
        }
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

        AlmostFBIMiner am(db, tau, errorTids);
        am.run();
        Repairer valueCl(db, m.getRules());
        valueCl.setAlmost(am.getRules(), tau);
        for (int tid : errorTids) {
            std::cout << "Dirty Transaction: ";
            Output::printItemset(db.getRow(tid), db);
            std::cout << "Repaired Transaction: ";
            Output::printItemset(valueCl.getRepair(tid), db);  
            std::cout << std::endl << "--------------" << std::endl;
        }
    }
}
