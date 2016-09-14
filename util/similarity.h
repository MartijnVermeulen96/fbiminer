#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "../data/database.h"

class Similarity {
public:
    static double lin(const Database&, const Transaction&, const Transaction&);
};

#endif // SIMILARITY_H
