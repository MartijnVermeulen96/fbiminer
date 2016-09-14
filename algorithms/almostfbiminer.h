#ifndef SRC_ALGORITHMS_ALMOSTFBIMINER_H_
#define SRC_ALGORITHMS_ALMOSTFBIMINER_H_

#include "baseminer.h"
#include "forbiddenitemset.h"

class AlmostFBIMiner : public BaseMiner {
public:
    AlmostFBIMiner(const Database&, double, const std::vector<int>&);
    void run();
    const std::vector<ForbiddenItemset>& getRules();

private:
    void mine(const Itemset&, const std::vector<MinerNode>&, const std::map<int,TidList>&);
    bool computeLift(const Itemset&, int, int);
    double minPossLift(int, int, int, int, int, int, int) const;
    double findMaxLift(const Itemset&, int&, int&, Itemset&, Itemset&) const;
    double maxout(int, int, int) const;
    TidList getDirtyTids(int) const;
    int getDirtySupp(const Itemset&) const;
    std::vector<int> filterSameAttr(const Itemset&, const std::vector<int>&) const;
    void buildSuffixes(const MinerNode&, MNIter, MNIter, const std::vector<int>&, const TidList&, 
        const std::map<int,TidList>&, std::vector<MinerNode>&, std::map<int, TidList>&) const;

private:
	double fMaxLift;
    std::vector<int> fDirties;
    std::vector<ForbiddenItemset> fAlmostFBI;
    PrefixTree<Itemset, int> fDirtySupps;
};
#endif // SRC_ALGORITHMS_ALMOSTFBIMINER_H_
