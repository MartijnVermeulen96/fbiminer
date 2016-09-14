#include "forbiddenitemset.h"
#include "../util/setutil.h"
#include <algorithm>

ForbiddenItemset::ForbiddenItemset(const Itemset& items, int supp = -1, double lift = -1)
    :fSupp(supp), fItems(items), fLift(lift) {

}

const Itemset& ForbiddenItemset::getItems() const {
    return fItems;
}

int ForbiddenItemset::nrItems() const {
    return fItems.size();
}

bool ForbiddenItemset::equals(const ForbiddenItemset& r) const {
    return fItems == r.fItems && fSupp == r.fSupp && fLift == r.fLift;
}

bool ForbiddenItemset::subsetOf(const ForbiddenItemset& r) const {
    return fSupp == r.fSupp && fLift == r.fLift && isSubsetOf(fItems, r.fItems);
}

bool ForbiddenItemset::contains(int x) const {
    return std::binary_search(fItems.begin(), fItems.end(), x);
}

bool ForbiddenItemset::match(const Transaction& tup) const {
    for (int i : fItems) {
        if (std::find(tup.begin(), tup.end(), i) == tup.end()) return false;
    }
    return true;
}

double ForbiddenItemset::getLift() const {
    return fLift;
}

int ForbiddenItemset::getSupp() const {
    return fSupp;
}

void ForbiddenItemset::getErrorIndices(const Transaction& tup, std::set<int>& indices) const {
    for (uint ix = 0; ix < tup.size(); ix++) {
        int item = tup[ix];
        if (std::binary_search(fItems.begin(), fItems.end(), item)) {
            indices.insert(ix);
        }
    }
}
