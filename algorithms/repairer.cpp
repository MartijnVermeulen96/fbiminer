#include "repairer.h"
#include "../util/similarity.h"
#include <algorithm>

Repairer::Repairer(Database& db, const FBIs& forbidden)
    :fDb(db) {
    for (unsigned i = 0; i < fDb.size(); i++) {
        const Transaction& tup = fDb.getRow(i);
        for (const auto& forbi : forbidden) {
            if (forbi.match(tup)) {
                fDirty[i].push_back(forbi);
            }
        }
        if (fDirty.find(i) == fDirty.end()){
            fClean.push_back(i);
        }
    }
}

void Repairer::setAlmost(const FBIs& almost, double tau) {
    fOld.clear();
    fPot.clear();
    for (const auto& forbi : almost) {
        if (forbi.getSupp() == 0 || forbi.getLift() < tau) {
            fOld.push_back(forbi);
        }
        else {
            fPot.push_back(forbi);
        }
    }
}

bool Repairer::safe1(const Transaction& trepi) const {
    for (const auto& oldFBI : fOld) {
        if (oldFBI.match(trepi)) {
            return false;
        }
    }
    return true;
}

bool Repairer::safe2(const Transaction& ti, const Transaction& trepi, const std::vector<int>& tiPot) const {
    std::vector<int> trepiPot = getMatchIxs(trepi, fPot);
    for (int ix : tiPot) {
        if (!std::binary_search(trepiPot.begin(), trepiPot.end(), ix)) {
            return false;
        }
    }
    std::vector<int> diff;
    for (int val : trepi) {
        if (std::find(ti.begin(), ti.end(), val) == ti.end()) {
            diff.push_back(val);
        }
    }
    for (int val : diff) {
        std::vector<int> diffSupers = getSuperIxs(val, fPot);
        for (int ix : diffSupers) {
            if (!std::binary_search(trepiPot.begin(), trepiPot.end(), ix)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<int> Repairer::getMatchIxs(const Transaction& t, const FBIs& fbis) const {
    std::vector<int> indices;
    for (unsigned ix = 0; ix < fbis.size(); ix++) {
        const ForbiddenItemset& forbi = fbis[ix];
        if (forbi.match(t)) {
            indices.push_back(ix);
        }
    }
    return indices;
}

std::vector<int> Repairer::getSuperIxs(int d, const FBIs& fbis) const {
    std::vector<int> indices;
    for (unsigned ix = 0; ix < fbis.size(); ix++) {
        const ForbiddenItemset& forbi = fbis[ix];
        if (forbi.contains(d)) {
            indices.push_back(ix);
        }
    }
    return indices;
}

std::vector<Repairer::nnEntry> Repairer::getNearestNeighbours(const Transaction& t) const {
    std::vector<nnEntry> neighbours;
    neighbours.reserve(fClean.size());
    for (unsigned cleanIx : fClean) {
        const Transaction& cleanTuple = fDb.getRow(cleanIx);
        double dist = 1.0 / Similarity::lin(fDb, t, cleanTuple);
        neighbours.push_back(nnEntry(dist, cleanIx));
    }
    std::sort(neighbours.begin(), neighbours.end(),
        [](const nnEntry& a, const nnEntry& b)
        {
            return a.first < b.first;
        });
    return neighbours;
}

Transaction Repairer::getRepair(int row) const {
    const Transaction& ti = fDb.getRow(row);
    std::vector<int> tiOld = getMatchIxs(ti, fOld);
    std::vector<int> tiPot = getMatchIxs(ti, fPot);
    std::set<Transaction> attempted;
    std::set<int> errorIxSet;
    for (const auto& forb : fDirty.at(row)) {
        forb.getErrorIndices(ti, errorIxSet);
    }
    std::vector<int> errorIxs(errorIxSet.begin(), errorIxSet.end());
    std::vector<nnEntry> neighbours = getNearestNeighbours(ti);
    for (const nnEntry& nne : neighbours) {
        Transaction trepi = ti;
        const Transaction& nn = fDb.getRow(nne.second);
        for (unsigned i = 0; i < trepi.size(); i++) {
            if (std::find(errorIxs.begin(), errorIxs.end(), i) != errorIxs.end()) {
                trepi[i] = nn[i];
            }
        }
        if (attempted.find(trepi) == attempted.end()) {
            attempted.insert(trepi);
            if (safe1(trepi) && safe2(ti,trepi,tiPot)) {
                return trepi;
            }
        }  
    }
    return ti;
}