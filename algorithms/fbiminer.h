#ifndef SRC_ALGORITHMS_FBIMINER_H_
#define SRC_ALGORITHMS_FBIMINER_H_

#include "baseminer.h"
#include "forbiddenitemset.h"

const int SUPP_PRUNING = 1;
const int SUPP_DIFF_PRUNING = 2;
const int LIFT_DENOM_PRUNING = 4;
const int FREQ_BOUND = 8;

class FBIMiner : public BaseMiner {
public:
    FBIMiner(const Database&, double, int pruning = SUPP_PRUNING | LIFT_DENOM_PRUNING | SUPP_DIFF_PRUNING | FREQ_BOUND);
	void run();
    const std::vector<ForbiddenItemset>& getRules() const;

protected:
    void mine(const Itemset&, const std::vector<MinerNode>&);
    bool computeLift(const Itemset&, int);
    std::vector<MinerNode> buildSuffix(const MinerNode&, MNIter, MNIter, std::vector<int>, double) const;
    double lift(const Itemset&, int) const;

private:
	double fMaxLift;
    double fFreqBound;
    int fPruning;
    std::vector<ForbiddenItemset> fForbidden;

public:
	int fNrVisited;
};
#endif // SRC_ALGORITHMS_FBIMINER_H_
