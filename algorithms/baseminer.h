#ifndef ALGORITHMS_BASEMINER_H_
#define ALGORITHMS_BASEMINER_H_

#include <map>
#include <vector>
#include "minernode.h"
#include "../util/prefixtree.h"

class BaseMiner {
public:
    BaseMiner(const Database&);

protected:
    typedef std::vector<MinerNode>::const_iterator MNIter;
    
    std::vector<MinerNode> getSingletons(int);
    void sortNodes(std::vector<MinerNode>&) const;
    std::vector<TidList> bucketTids(const std::vector<int>&, const TidList&) const;

    bool addMinGen(const Itemset&, int, int);
    int sigmax(const Itemset&) const;
    int getSupport(const Itemset&) const;
    void storeSupport(const Itemset&, int);

protected:
	const Database& fDb;
    PrefixTree<Itemset, int> fSupports;

private:
    std::vector<std::pair<int,int>> fItemFrequencies;
    std::map<int, PrefixTree<Itemset,int>> fGenMap;
};
#endif // ALGORITHMS_BASEMINER_H_
