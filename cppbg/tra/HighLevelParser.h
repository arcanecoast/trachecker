#ifndef CPPBG_TRA_HIGHLEVELPARSER 
#define CPPBG_TRA_HIGHLEVELPARSER

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 * 
 * Parser for WeiDU .tra files.
 */

#include <map>
#include <boost/shared_ptr.hpp>

#include "TranslationEntry.h"

using boost::shared_ptr;
using std::map;

namespace cppbg_tra
{
    class HighLevelParserImpl;

	class HighLevelParser
	{
    public:
        typedef map<int, shared_ptr<TranslationEntry> >::iterator iterator;
        typedef map<int, shared_ptr<TranslationEntry> >::const_iterator const_iterator;

    public:
        HighLevelParser();

        void LoadFromFile(const char* Filename);
        void SaveToFile(const char* Filename) const;

        size_t Size() const;

        const_iterator GetEntry(int ID) const;
        iterator GetEntry(int ID);

        iterator Begin();
        iterator End();

        const_iterator Begin() const;
        const_iterator End() const;

        void Clear();

    private:
        shared_ptr<HighLevelParserImpl> pimpl;
	};
};

#endif // CPPBG_TRA_HIGHLEVELPARSER