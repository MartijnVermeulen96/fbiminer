#include "baseminer.h"
#include "../util/setutil.h"

BaseMiner::BaseMiner(const Database& db)
    :fDb(db) {
}

void BaseMiner::sortNodes(std::vector<MinerNode>& nodes) const {
    // Sort itemsets by increasing support
    std::sort(nodes.begin(), nodes.end(),
        [](const MinerNode& a, const MinerNode& b)
        {
            return a.fTids.size() < b.fTids.size();
        });
}

std::vector<MinerNode> BaseMiner::getSingletons(int minsup) {
    std::vector<MinerNode> singletons;
    std::unordered_map<int, int> nodeIndices;
    for (unsigned item = 1; item <= fDb.nrItems(); item++) {
        if (fDb.frequency(item) >= minsup) {
            singletons.push_back(MinerNode(item, fDb.frequency(item)));
            nodeIndices[item] = singletons.size() - 1;
        }
    }

    for (unsigned row = 0; row < fDb.size(); row++) {
        const auto& tup = fDb.getRow(row);
        for (int item : tup) {
            const auto& it = nodeIndices.find(item);
            if (it != nodeIndices.end()) {
                singletons[it->second].fTids.push_back(row);
            }
        }
    }
    
    sortNodes(singletons);
    fItemFrequencies.reserve(singletons.size());
    for (int i = singletons.size() - 1; i >= 0; i--) {
        auto& node = singletons[i];
        node.hashTids();
        fItemFrequencies.push_back(std::make_pair(node.fItem, node.fSupp));
    }
    return singletons;
}

bool BaseMiner::addMinGen(const Itemset& newset, int supp, int hash) {
    const auto& genIt = fGenMap.find(hash);
    if (genIt != fGenMap.end()) {
        if (genIt->second.hasSubset(newset, supp)) {
            return false;
        }
        genIt->second.insert(newset, supp);
    }
    else {
        fGenMap[hash].insert(newset, supp);
    }
    return true;
}

std::vector<TidList> BaseMiner::bucketTids(const std::vector<int>& items, const TidList& nodeTids) const {
    std::vector<TidList> ijtids(items.size());
    for (int ix = 0; ix < items.size(); ix++) {
        for (int t : nodeTids) {
            const Transaction& tup = fDb.getRow(t);
            if (std::find(tup.begin(), tup.end(), items[ix]) != tup.end()) {
                ijtids[ix].push_back(t);
            }
        }
    }
    return ijtids;
}

int BaseMiner::sigmax(const Itemset& items) const {
    std::unordered_set<int> attrs;
    attrs.reserve(items.size());
    for (int i : items) {
        attrs.insert(fDb.getAttrIndex(i));
    }
    for (const auto& itemFreqPair : fItemFrequencies) {
        bool attrMatch = false;
        if (attrs.find(fDb.getAttrIndex(itemFreqPair.first)) == attrs.end()) {
            return itemFreqPair.second;
        }
        if (std::binary_search(items.begin(), items.end(), itemFreqPair.first)) {
            return itemFreqPair.second;
        }
    }
    return -1;
}

int BaseMiner::getSupport(const Itemset& items) const {
    const int* iter = fSupports.find(items);
    return iter ? *iter : -1;
}

void BaseMiner::storeSupport(const Itemset& items, int supp) {
    fSupports.insert(items, supp);
}