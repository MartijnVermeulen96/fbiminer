#ifndef SRC_DATA_DATABASE_H_
#define SRC_DATA_DATABASE_H_

#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <fstream>

typedef std::vector<int> Transaction;

struct pairhash {
public:
	template <typename T1, typename T2> 
	size_t operator()(const std::pair<T1,T2>& p) const
	{
		return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
	}
};

class Database {
public:
	Database();
	unsigned size() const;
	unsigned nrAttrs() const;
    unsigned nrItems() const;

    const Transaction& getRow(int) const;
    void setRow(int, const Transaction&);
	void write(std::ofstream& file, char delim=' ') const;
	void sort();

	int getAttrIndex(int t) const;
	int frequency(int i) const;
	int maxSupp() const;
	const std::string& getValue(int i) const;
	const std::vector<int>& getDomainOfItem(int) const;
	const std::vector<int>& getDomain(int) const;
    const std::string& getAttrName(int) const;

protected:
    void setAttributes(const std::vector<std::string>&);
    void incFreq(int i);
    void addRow(const Transaction&);
    int translateToken(int, const std::string&);

private:
	struct AttributeInfo {
		AttributeInfo(const std::string& name):fName(name) { }
		std::string fName;
		std::vector<int> fValues;
	};
	struct ItemInfo {
		ItemInfo(const std::string& val, int attr):fValue(val), fAttribute(attr), fFrequency(0) { }
		std::string fValue;
		int fAttribute;
		int fFrequency;
	};
	std::vector<Transaction> fData; //array of data represented as rows of integers
	std::unordered_map<std::pair<int, std::string>, int, pairhash> fItemDictionary; // maps a value string for the given attribute index to corresponding item id
	std::vector<AttributeInfo> fAttributes;
	std::vector<ItemInfo> fItems;
	int fNumTokens;
};

#endif // SRC_DATA_DATABASE_H_
