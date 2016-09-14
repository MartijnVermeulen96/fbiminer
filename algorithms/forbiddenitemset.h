#ifndef SRC_ALGORITHMS_FORBIDDENITEMSET_H_
#define SRC_ALGORITHMS_FORBIDDENITEMSET_H_

#include "../data/database.h"
#include "minernode.h"
#include <set>

class ForbiddenItemset {
public:
    ForbiddenItemset(const Itemset&, int, double);
    int nrItems() const;
    double getLift() const;
    int getSupp() const;
    const Itemset& getItems() const;
    bool equals(const ForbiddenItemset&) const;
    bool subsetOf(const ForbiddenItemset&) const;
    bool contains(int) const;
    bool match(const Transaction&) const;
    void getErrorIndices(const Transaction&, std::set<int>&) const;

private:
    int fSupp;
    Itemset fItems;
    double fLift;
};

#endif // SRC_ALGORITHMS_FORBIDDENITEMSET_H_
