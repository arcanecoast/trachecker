#ifndef CPPBG_TRA_HIGHLEVELPARSERIMPL 
#define CPPBG_TRA_HIGHLEVELPARSERIMPL

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 * 
 * Parser for WeiDU .tra files.
 */

#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "LowLevelParser.h"
#include "TranslationEntry.h"

using std::string;
using std::map;
using std::vector;
using boost::shared_ptr;

namespace cppbg_tra
{
	class HighLevelParserImpl
	{
    public:
        typedef map<int, shared_ptr<TranslationEntry> >::iterator iterator;
        typedef map<int, shared_ptr<TranslationEntry> >::const_iterator const_iterator;

    public:
        void LoadFromFile(const char* Filename);
        void SaveToFile(const char* Filename) const;

        size_t Size() const { return this->entries.size(); }

        const_iterator GetEntry(int ID) const { return this->entries.find(ID); }
        iterator GetEntry(int ID) { return this->entries.find(ID); }

        iterator Begin() { return this->entries.begin(); }
        iterator End() { return this->entries.end(); }

        const_iterator Begin() const { return this->entries.begin(); }
        const_iterator End() const { return this->entries.end(); }

        void Clear() { this->entries.clear(); }

    private:
        struct LinkToEntry {
            int id;
            int idAssignedTo;
            LowLevelParser::iterator referenceToAssignee;

            LinkToEntry(int Id, int IdAssignedTo, LowLevelParser::iterator ReferenceToAssignee) : 
                id(Id), idAssignedTo(IdAssignedTo), referenceToAssignee(ReferenceToAssignee) {}
        };

    private:
        void CheckStack(LowLevelParser::iterator newElement, vector<LowLevelParser::iterator>& stack);
        void AggregateEntry(int EntryID, vector<LowLevelParser::iterator>& stack);
        int GetIntID(LowLevelParser::iterator element);

    private:
        map<int, shared_ptr<TranslationEntry> > entries;
	};
};

#endif // CPPBG_TRA_HIGHLEVELPARSERIMPL