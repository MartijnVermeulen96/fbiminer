#ifndef SRC_ALGORITHMS_REPAIRER_H_
#define SRC_ALGORITHMS_REPAIRER_H_

#include "fbiminer.h"

class Repairer {
    typedef std::vector<ForbiddenItemset> FBIs;
    typedef std::pair<double, int> nnEntry;
public:
    Repairer(Database&, const FBIs&);
    void setAlmost(const FBIs&, double);    
    Transaction getRepair(int) const;

protected:
    bool safe1(const Transaction&) const;
    bool safe2(const Transaction&, const Transaction&, const std::vector<int>&) const;
    std::vector<int> getMatchIxs(const Transaction&, const FBIs&) const;
    std::vector<int> getSuperIxs(int, const FBIs&) const;

    std::vector<nnEntry> getNearestNeighbours(const Transaction&) const;

private:
	Database& fDb;
    std::vector<ForbiddenItemset> fOld;
    std::vector<ForbiddenItemset> fPot;
	std::vector<int> fClean;
    std::unordered_map<int, std::vector<ForbiddenItemset>> fDirty;
};

#endif // SRC_ALGORITHMS_REPAIRER_H_
