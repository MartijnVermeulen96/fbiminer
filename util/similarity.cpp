#include "similarity.h"
#include <cmath>

double Similarity::lin(const Database& db, const Transaction& x, const Transaction& y) {
    double denom = 0.0;
    double num = 0.0;
    for (uint i = 0; i < x.size(); i++) {
        denom += std::log(db.frequency(x[i])/(double)db.size()) + std::log(db.frequency(y[i])/(double)db.size());
        if (x[i] == y[i]) {
            num += 2 * std::log(db.frequency(x[i])/(double)db.size());
        }
        else {
            num += 2 * std::log((db.frequency(x[i]) + db.frequency(y[i]))/(double)db.size());
        }
    }
    return num/denom;
}
