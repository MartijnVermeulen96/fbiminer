#include "almostfbiminer.h"
#include "../util/setutil.h"
#include "../util/output.h"
#include <iostream>

AlmostFBIMiner::AlmostFBIMiner(const Database& db, double maxLift, const std::vector<int>& dirties)
    :BaseMiner(db), fMaxLift(maxLift), fDirties(dirties) {
}

void AlmostFBIMiner::run() {
    const auto& singles = getSingletons((1.0 * fDb.size())/(fDb.maxSupp() * fMaxLift) - fDirties.size());
    std::map<int, TidList> dirtySingles;
    //dirtySingles.reserve(singles.size());
    for (const auto& node : singles) {
        dirtySingles[node.fItem] = getDirtyTids(node.fItem);
    }
    fSupports.reserve(singles.size());
    mine(Itemset(), singles, dirtySingles);
}

const std::vector<ForbiddenItemset>& AlmostFBIMiner::getRules() {
    return fAlmostFBI;
}

TidList AlmostFBIMiner::getDirtyTids(int item) const {
    TidList tids;
    for (int dirtyIx : fDirties) {
        const auto& dirt = fDb.getRow(dirtyIx);
        if (std::binary_search(dirt.begin(), dirt.end(), item)) {
            tids.push_back(dirtyIx);
        }
    }
    return tids;
}

int AlmostFBIMiner::getDirtySupp(const Itemset& items) const {
    const int* supp = fDirtySupps.find(items);
    return supp ? *supp : -1;
}

void AlmostFBIMiner::buildSuffixes( const MinerNode& node, MNIter begin, MNIter end, const std::vector<int>& rightItems, const TidList& dirtyNode, 
        const std::map<int,TidList>& dirtyTids, std::vector<MinerNode>& suffix, std::map<int, TidList>& dirtySuffix) const {
    int suppDiffB = 1.0/fMaxLift - 1 - fDirties.size();
    if (rightItems.size() > 2 * fDb.nrAttrs()) {
        std::vector<TidList> ijtidMap = bucketTids(rightItems, node.fTids);
        for (uint jx = 0; jx < rightItems.size(); jx++) {
            int jtem = rightItems[jx];
            int mSupp = node.fSupp - dirtyNode.size();
            const auto& newTids = ijtidMap.at(jx);
            int nSupp = newTids.size();
            if (mSupp - nSupp >= suppDiffB) {
                const auto& newDirtyTids = intersection(dirtyNode, dirtyTids.at(jtem));
                nSupp = newTids.size() - newDirtyTids.size();
                if (mSupp - nSupp >= suppDiffB) {
                    suffix.push_back(MinerNode(jtem, std::move(newTids)));
                    dirtySuffix[jtem] = newDirtyTids;
                }
            }
        }
    }
    else {
        for (; begin != end; begin++) {
            const MinerNode& j = *begin;
            int mSupp = node.fSupp - dirtyNode.size();
            const auto& newTids = intersection(node.fTids, j.fTids);
            int nSupp = newTids.size();
            if (mSupp - nSupp >= suppDiffB) {
                const auto& newDirtyTids = intersection(dirtyNode, dirtyTids.at(j.fItem));
                nSupp = newTids.size() - newDirtyTids.size();
                if (mSupp - nSupp >= suppDiffB) {
                    suffix.push_back(MinerNode(j.fItem, newTids));
                    dirtySuffix[j.fItem] = newDirtyTids;
                }
            }
        }
    }
}

void AlmostFBIMiner::mine(const Itemset& prefix, const std::vector<MinerNode>& items, const std::map<int,TidList>& dirtyItems) {
    std::vector<int> rightItems;
    rightItems.reserve(items.size());
    for (int ix = items.size()-1; ix >= 0; ix--) {
        const MinerNode& node = items[ix];
        Itemset newset = join(prefix, node.fItem);
        double sm = sigmax(newset) + fDirties.size();
        double dsm = (double)fDb.size() / sm;
        const TidList& dirtyTids = dirtyItems.at(node.fItem);
        int dirtysupp = dirtyTids.size();
        int cleansupp = node.fSupp - dirtysupp;
        double suppDiffB = 1.0/fMaxLift - ((double)fDb.maxSupp()/(double)fDb.size()) - fDirties.size();

        if (suppDiffB >= 1 && !addMinGen(newset, node.fSupp, node.fHash)) continue;
        if (newset.size() > 1 && !computeLift(newset, node.fSupp, cleansupp)) continue;
        if (cleansupp < (dsm/fMaxLift) - fDirties.size()) continue;

        storeSupport(newset, node.fSupp);
        fDirtySupps.insert(newset, dirtysupp);
        if (rightItems.size()) {
            std::vector<MinerNode> suffix;
            std::map<int, TidList> dirtySuffix;
            buildSuffixes(node, items.begin() + ix + 1, items.end(), filterSameAttr(newset, rightItems), dirtyTids, dirtyItems, suffix, dirtySuffix);
            if (suffix.size()) {
                sortNodes(suffix);
                mine(newset, suffix, dirtySuffix);
            }
        }
        rightItems.push_back(node.fItem);
    }
}

bool AlmostFBIMiner::computeLift(const Itemset& newItems, int supp, int cleansupp) {
    int a, b;
    Itemset ai, bi;
    double ld = findMaxLift(newItems, a, b, ai, bi);
    if (ld < 0) return false;
    if (supp) {
        int acleansupp = a - getDirtySupp(ai);
        int bcleansupp = b - getDirtySupp(bi);
        if (cleansupp) {
            double mpl = minPossLift(a, acleansupp, b, bcleansupp, supp, cleansupp, fDirties.size());
            if (mpl <= fMaxLift) {
                fAlmostFBI.push_back(ForbiddenItemset(newItems, supp, ld * supp));
            }
            double mplbound = (double)fDb.size() / (double)((acleansupp + fDirties.size()) * (bcleansupp + fDirties.size()));
            if (mplbound > fMaxLift) return false;
        }
        else {
            // TODO
        }
    }
    else {
        double mpl = ((double)fDb.size()) / maxout(a, b, fDirties.size() - 1);   
        if (mpl <= fMaxLift) { 
            fAlmostFBI.push_back(ForbiddenItemset(newItems, supp, -1));
            return false;
        }
    }
    return true;
}

double AlmostFBIMiner::findMaxLift(const Itemset& itemset, int& as, int& bs, Itemset& ai, Itemset& bi) const {
    if (itemset.size() == 2) {
        as = fDb.frequency(itemset[0]);
        ai.push_back(itemset[0]);
        bs = fDb.frequency(itemset[1]);
        bi.push_back(itemset[1]);
        return 1.0 / (double)(as * bs);
    }
    double maxlift = 0;
    const uint halfk = itemset.size() >> 1;

    for (uint k = 1; k <= halfk; k++) {
        const int halfi = binomialCoeff(itemset.size(), k);
        SubsetIterator gen(itemset.size(), k);
        for (int aix = 0; aix < halfi; aix++) {
            std::bitset<32> amask = gen.next();
            const auto asub = subset(itemset, amask);
            int asupp = getSupport(asub);
            if (asupp > 0) {
                const auto bsub = subset(itemset, amask.flip());
                int bsupp = getSupport(bsub);
                if (bsupp > 0) {
                    const double lift = 1.0 / (double)(asupp * bsupp);
                    if (lift > maxlift) {
                        maxlift = lift;
                        as = asupp;
                        ai = asub;
                        bs = bsupp;
                        bi = bsub;
                    }
                    continue;
                }
            }
            return -1;
        }
    }
    return maxlift;

    /*double maxlift = 0;
    const uint halfk = itemset.size() >> 1;
    //const auto& supps = fSupports.getSupports(itemset);
    for (uint k = 1; k <= halfk; k++) {
        const int halfi = binomialCoeff(itemset.size(), k);
        SubsetIterator gen(itemset.size(), k);
        for (int aix = 0; aix < halfi; aix++) {
            std::bitset<32> amask = gen.next();
            const auto asub = subset(itemset, amask);
            const auto& asuppit = supps.find(asub);
            int asupp = asuppit != supps.end() ? asuppit->second : -1;
            //getSupport(asub);
            if (asupp > 0) {
                const auto bsub = subset(itemset, amask.flip());
                const auto& bsuppit = supps.find(bsub);
                int bsupp = bsuppit != supps.end() ? bsuppit->second : -1;
                //int bsupp = getSupport(bsub);
                if (bsupp > 0) {
                    const double lift = 1.0 / (double)(asupp * bsupp);
                    if (lift > maxlift) {
                        maxlift = lift;
                        as = asupp;
                        ai = asub;
                        bs = bsupp;
                        bi = bsub;
                    }
                    continue;
                }
            }
            return -1;
        }
    }
    return maxlift;*/
}

double AlmostFBIMiner::maxout(int a, int b, int nrDirty) const {
    if (a > b) {
        std::swap(a, b);
    }
    if (a <= b - fDirties.size()) {
        return b * (a + fDirties.size());
    }
    int half = (b + a + nrDirty) >> 1;
    return half * ((b + a + nrDirty) - half);
}

std::vector<int> AlmostFBIMiner::filterSameAttr(const Itemset& newset, const std::vector<int>& items) const {
    std::vector<int> attrs;
    attrs.reserve(newset.size());
    std::vector<int> rightItems;
    for (int i : newset) {
        attrs.push_back(fDb.getAttrIndex(i));
    }
    for (int i : items) {
        if (std::find(attrs.begin(), attrs.end(), fDb.getAttrIndex(i)) == attrs.end()) {
            rightItems.push_back(i);
        }
    }
    return rightItems;
}

double AlmostFBIMiner::minPossLift(int a, int ac, int b, int bc, int ab, int abc, int epsilon) const {
    while (epsilon) {
        if (a > b) {
            std::swap(a,b);
        }
        if (ab > abc) {
            const double c1 = (ab - 1) / (double)(a*(b - 1));
            const double c2 = 1 / (double)(a+b);
            if (c1 <= c2) {
                b--;
                ab--;
                epsilon--;
                continue;
            }
        }
        if (a < ac + fDirties.size()) {
            a++;
            epsilon--;
        }
        else if (b < bc + fDirties.size()) {
            b++;
            epsilon--;
        }
        else {
            break;
        }
    }
    return fDb.size() * (double)ab / (double)(a*b);
}