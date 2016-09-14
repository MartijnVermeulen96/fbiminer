#ifndef SRC_UTIL_PREFIXTREE_H
#define SRC_UTIL_PREFIXTREE_H

#include <map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <algorithm>

template<typename T>
size_t hashCollection(const T& xs) {
	size_t res = 0;
	for (const typename T::value_type& x : xs) {
		res ^= std::hash<typename T::value_type>()(x) + 0x9e3779b9 + (res << 6) + (res >> 2);
	}
	return res;
}

template <typename Key, typename Value>
class PrefixTree {
public:
	PrefixTree();
	void reserve(int);
	void insert(const Key&, const Value&);
	const Value* find(const Key&) const;
	bool hasSubset(const Key&, const Value&) const;
	std::map<Key, int> getSupports(const Key&) const;
private:
	struct PrefixNode {
		typename Key::value_type fValue;
		std::map<typename Key::value_type, PrefixNode> fSubTrees;
		PrefixNode* fParent;
		typename Key::value_type fKey;
		int fDepth;
	};
private:
	int size;
	PrefixNode fRoot;
	std::unordered_set<size_t> fHashes;
	std::unordered_map<size_t, std::vector<PrefixNode*> > fJumps;
};

template <typename Key, typename Value>
PrefixTree<Key,Value>::PrefixTree() {
	fRoot.fDepth = 0;
	fRoot.fValue = -1;
}

template <typename Key, typename Value>
void PrefixTree<Key,Value>::reserve(int r) {
	size = r;
	//fRoot.fSubTrees.reserve(r);
}

template <typename Key, typename Value>
void PrefixTree<Key,Value>::insert(const Key& k, const Value& v) {
	typename Key::const_iterator it = k.begin();
	PrefixNode* insertionPoint = &fRoot;
	while (it != k.end()) {
		PrefixNode* next = &insertionPoint->fSubTrees[*it];
		if (!next->fParent) {
			next->fKey = *it;
			next->fDepth = insertionPoint->fDepth + 1;
			next->fValue = -1;
			next->fParent = insertionPoint;
		}
		insertionPoint = next;
		it++;
	}
	fJumps[hashCollection(k)].push_back(insertionPoint);
	insertionPoint->fValue = v;
}

template <typename Key, typename Value>
const Value* PrefixTree<Key,Value>::find(const Key& k) const {
	const auto& aaa = fJumps.find(hashCollection(k));
	if (aaa == fJumps.end()) return 0;

	for (PrefixNode* pn : aaa->second) {
		if (k.size() != pn->fDepth) continue;
		PrefixNode* src = pn;
		typename Key::const_reverse_iterator rit = k.rbegin();
		while (rit != k.rend() && pn && pn->fKey == *rit) {
			pn = pn->fParent;
			rit++;
		}
		if (rit == k.rend() && pn == &fRoot) return &src->fValue;
	}
	return 0;

	typename Key::const_iterator it = k.begin();
	const PrefixNode* findPoint = &fRoot;
	while (it != k.end()) {
		const auto sub = findPoint->fSubTrees.find(*it);
		if (sub != findPoint->fSubTrees.end()) {
			findPoint = &(sub->second);
			it++;
		}
		else {
			return 0;
		}
	}
	return &findPoint->fValue;
}

template <typename Key, typename Value>
bool PrefixTree<Key,Value>::hasSubset(const Key& k, const Value& v) const {
	std::queue<const PrefixNode*> fringe;
	fringe.push(&fRoot);
	while (!fringe.empty()) {
		const PrefixNode* elem = fringe.front();
		fringe.pop();
		if (elem->fValue == v) return true;
		for (const auto& sub : elem->fSubTrees) {
			if (std::binary_search(k.begin(), k.end(), sub.first)) {
				fringe.push(&sub.second);
			}
		}
	}
	return false;
}

template <typename Key, typename Value>
std::map<Key, int> PrefixTree<Key,Value>::getSupports(const Key& k) const {
	std::map<Key, int> result;
	std::queue<std::pair<Key, const PrefixNode*> > fringe;
	fringe.push(std::make_pair(Key(), &fRoot));
	while (!fringe.empty()) {
		const PrefixNode* elem = fringe.front().second;
		const Key& pk = fringe.front().first;
		for (const auto& sub : elem->fSubTrees) {
			if (std::binary_search(k.begin(), k.end(), sub.first)) {
				Key newk(pk.begin(), pk.end());
				newk.push_back(sub.first);
				result[newk] = sub.second.fValue;
				fringe.push(std::make_pair(newk, &sub.second));
			}
		}
		fringe.pop();
	}
	return result;
}

#endif //SRC_UTIL_PREFIXTREE_H