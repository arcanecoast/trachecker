#ifndef CPPBG_TRA_LOWLEVELPARSER 
#define CPPBG_TRA_LOWLEVELPARSER

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 */

#include <string>
#include <list>
#include <fstream>

#include "Position.h"

using std::string;
using std::list;
using std::ifstream;

namespace cppbg_tra
{
	class LowLevelElement
	{
    public:
        enum EntityType {
            ENTITY_ID    = 0,
            ENTITY_TEXT  = 1,
            ENTITY_SOUND = 2,
            ENTITY_MISC  = 3
        };

	public:
        explicit LowLevelElement(EntityType ElementType, const string& Content);
        explicit LowLevelElement(EntityType ElementType, const string& Content,
            int IndexOfContentPlacementLine, int IndexOfContentPlacementInLine,
            int OffsetToContentPlacement);
        explicit LowLevelElement(EntityType ElementType, const string& Content,
            const Position& Pos);

        void SetContent(const string& Content) { this->content = Content; }

        string GetWritableData() const;

        string GetContent() const { return this->content; }
        EntityType GetType() const { return this->type; }
        Position GetPosition() const { return this->position; }

    private:
        Position                    position;
        string                      content;
        LowLevelElement::EntityType type;
	};

	class LowLevelParser
	{
    public:
        typedef list<LowLevelElement>::iterator iterator;
        typedef list<LowLevelElement>::const_iterator const_iterator;

	public:
		void LoadFromFile(const char* Filename);
		void SaveToFile(const char* Filename) const;

        iterator Begin() { return m_entries.begin(); }
        iterator End() { return m_entries.end(); }

        const_iterator Begin() const { return m_entries.begin(); }
        const_iterator End() const { return m_entries.end(); }

        void Clear() { m_entries.clear(); }

    private:
        char GetChar(ifstream& in);

    private:
        list<LowLevelElement> m_entries;

        int m_currentLineIndex;
        int m_currentLineStart;

        Position m_newElementPosition;
	};
};

#endif // CPPBG_TRA_LOWLEVELPARSER