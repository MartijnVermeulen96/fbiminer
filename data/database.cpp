#include "database.h"
#include <sstream>
#include <iostream>
#include <algorithm>

Database::Database()
    :fNumTokens(1) {

}

unsigned Database::size() const {
    return fData.size();
}

unsigned Database::nrAttrs() const {
    return fAttributes.size();
}

unsigned Database::nrItems() const {
    return fItems.size();
}

const Transaction& Database::getRow(int row) const {
    return fData.at(row);
}

void Database::setRow(int row, const Transaction& newTup) {
    for (int i : fData.at(row)) {
        fItems[i - 1].fFrequency--;
    }
    for (int i : newTup) {
        fItems[i - 1].fFrequency++;
    }
    fData[row] = newTup;
}

void Database::addRow(const Transaction& t) {
    fData.push_back(t);
}

void Database::setAttributes(const std::vector<std::string>& attrs) {
    fAttributes.reserve(attrs.size());
    for (int i = 0; i < attrs.size(); i++) {
        fAttributes.emplace_back(attrs[i]);
    }
}

const std::vector<int>& Database::getDomainOfItem(int item) const {
    return fAttributes.at(fItems[item - 1].fAttribute).fValues;
}

const std::vector<int>& Database::getDomain(int attr) const {
    return fAttributes.at(attr).fValues;
}

int Database::translateToken(int attr, const std::string& strVal) {
    int& val = fItemDictionary[std::make_pair(attr, strVal)];
    if (val > 0) return val;

    fItems.emplace_back(strVal, attr);
    fAttributes[attr].fValues.push_back(fNumTokens);
    return val = fNumTokens++;
}

void Database::sort() {
    std::sort(fData.begin(), fData.end(),
        [](const Transaction& a, const Transaction& b)
        {
            for (int i = 0; i < a.size(); i++) {
                if (a[i] < b[i]) return true;
                if (b[i] < a[i]) return false;
            }
            return false;
        });
}

void Database::incFreq(int i) {
    fItems[i - 1].fFrequency++;
}

int Database::maxSupp() const {
    int max = 0;
    for (const auto& item : fItems) {
        if (item.fFrequency > max) {
            max = item.fFrequency;
        }
    }
    return max;
}

int Database::frequency(int i) const {
    return fItems[i - 1].fFrequency;
}

int Database::getAttrIndex(int i) const {
    return fItems[i - 1].fAttribute;
}

void Database::write(std::ofstream& file, char delim) const {
    for (const auto& row : fData) {
        for (const int& i : row) {
            file << i << delim;
        }
        file << std::endl;
    }
}

const std::string& Database::getAttrName(int i) const {
    return fAttributes[i].fName;
}

const std::string& Database::getValue(int i) const {
    return fItems[i - 1].fValue;
}
