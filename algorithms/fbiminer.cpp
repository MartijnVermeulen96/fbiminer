#include "fbiminer.h"
#include "../util/setutil.h"
#include "../util/output.h"
#include <iostream>
#include <numeric>

FBIMiner::FBIMiner(const Database& db, double maxLift, int pruning)
    :BaseMiner(db), fMaxLift(maxLift), fPruning(pruning), fNrVisited(0) {
}

void FBIMiner::run() {
    fNrVisited = 0;
    fFreqBound = (fPruning & FREQ_BOUND) ? (2 / fMaxLift) - 2 * std::sqrt((1 / (fMaxLift * fMaxLift)) - (1 / fMaxLift)) - 1 : 1;
    std::vector<MinerNode> singletons = getSingletons((fPruning & SUPP_PRUNING) ? (1.0 * fDb.size())/(fDb.maxSupp() * fMaxLift) : 1);
    mine(Itemset(), singletons);
}

const std::vector<ForbiddenItemset>& FBIMiner::getRules() const {
    return fForbidden;
}

void FBIMiner::mine(const Itemset& prefix, const std::vector<MinerNode>& items) {
    std::vector<int> rightItems;
    rightItems.reserve(items.size());
    for (int ix = items.size()-1; ix >= 0; ix--) {
        fNrVisited++;
        const MinerNode& node = items[ix];
        const Itemset newset = join(prefix, node.fItem);
        double sigm = sigmax(newset);
        if (!(fPruning & SUPP_DIFF_PRUNING) || addMinGen(newset, node.fSupp, node.fHash)) {
            if (newset.size() > 1 && !computeLift(newset, node.fSupp)) continue;
            if (fPruning & SUPP_PRUNING) {
                if (node.fSupp < (1.0 * fDb.size())/(sigm * fMaxLift)) continue;
            }
            storeSupport(newset, node.fSupp);

            if (rightItems.size()) {
                std::vector<MinerNode> suffix = buildSuffix(node, items.begin() + ix + 1, items.end(), rightItems, sigm);
                if (suffix.size()) {
                    sortNodes(suffix);
                    mine(newset, suffix);
                }
            }
            rightItems.push_back(node.fItem);
        }
    }
}

bool FBIMiner::computeLift(const Itemset& items, int supp) {
    if ((double)supp / fDb.size() > fFreqBound) return true;
    double l = lift(items, supp);
    if (l < 0) {
        return false;
    }
    if (l <= fMaxLift) {
        fForbidden.push_back(ForbiddenItemset(items, supp, l));
        return false;
    }
    if (fPruning & LIFT_DENOM_PRUNING) {
        if (l/supp > fMaxLift) {
            return false;
        }
    }
    return true;
}

std::vector<MinerNode> FBIMiner::buildSuffix(const MinerNode& node, MNIter begin, MNIter end, std::vector<int> rightItems, double sigm) const {
    std::vector<MinerNode> suffix;
    double dtau = ((double)fDb.size()) / fMaxLift;
    int suppDiffB = 0.99 + 1.0/fMaxLift - (sigm / (double)fDb.size());
    if (rightItems.size() > 4 * fDb.nrAttrs()) {
        std::vector<TidList> ijtidMap = bucketTids(rightItems, node.fTids);
        for (uint jx = 0; jx < rightItems.size(); jx++) {
            int jtem = rightItems[jx];
            TidList& ijtids = ijtidMap[jx];
            int ijsupp = ijtids.size();
            if (ijsupp) {
                if (!(fPruning & SUPP_DIFF_PRUNING) || (node.fSupp - ijsupp >= suppDiffB)) {
                    if (true || node.fSupp * fDb.frequency(jtem) >= dtau) {
                        suffix.push_back(MinerNode(jtem, std::move(ijtids)));
                    } 
                }
            }
        }
    }
    else {
        for (; begin != end; begin++) {
            const MinerNode& j = *begin;
            if (std::find(rightItems.begin(), rightItems.end(), j.fItem) == rightItems.end()) continue;
            if (false && (node.fSupp * fDb.frequency(j.fItem) < dtau)) continue;
            TidList ijtids = intersection(node.fTids, j.fTids);
            int ijsupp = ijtids.size();
            if (ijsupp) {
                if (!(fPruning & SUPP_DIFF_PRUNING) || node.fSupp - ijsupp >= suppDiffB) {
                    suffix.push_back(MinerNode(j.fItem, ijtids));
                }
            }
        }
    }
    return suffix;
}

double FBIMiner::lift(const Itemset& itemset, int supp) const {
    double maxlift = 0;
    const uint halfk = itemset.size() >> 1;
    for (uint k = 1; k <= halfk; k++) {
        const int i = binomialCoeff(itemset.size(), k);
        SubsetIterator gen(itemset.size(), k);
        for (int aix = 0; aix < i; aix++) {            
            std::bitset<32> amask = gen.next();
            int asupp = getSupport(subset(itemset, amask));
            if (asupp > 0) {
                int bsupp = getSupport(subset(itemset, amask.flip()));
                if (bsupp > 0) {
                    const double lift = (1.0 * supp * fDb.size()) / (double)(asupp * bsupp);
                    if (lift > maxlift) {
                        maxlift = lift;
                    }
                    continue;
                }
            }
            return -1;
        }
    }
    return maxlift;
}
