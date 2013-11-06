#ifndef CPPBG_TRA_POSITION 
#define CPPBG_TRA_POSITION

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 */

namespace cppbg_tra
{
    class Position
    {
    public:
        Position() :
          indexOfLine(-1),
          indexOfCharacterInLine(-1),
          physicalOffset(-1)
        {
        }

        explicit Position(int IndexOfLine, int IndexOfCharacterInLine, int PhysicalOffset) :
            indexOfLine(IndexOfLine),
            indexOfCharacterInLine(IndexOfCharacterInLine),
            physicalOffset(PhysicalOffset)
        {
        }

        int GetIndexOfLine() const { return this->indexOfLine; }
        int GetPlaceInLine() const { return this->indexOfCharacterInLine; }
        int GetPhysicalOffset() const { return this->physicalOffset; }

        void SetIndexOfLine(int IndexOfLine) { this->indexOfLine = IndexOfLine; }
        void SetPlaceInLine(int IndexOfCharacterInLine) { this->indexOfCharacterInLine = IndexOfCharacterInLine; }
        void SetPhysicalOffset(int PhysicalOffset) { this->physicalOffset = PhysicalOffset; }

        void Update(int IndexOfLine, int IndexOfCharacterInLine, int PhysicalOffset)
        {
            SetIndexOfLine(IndexOfLine);
            SetPlaceInLine(IndexOfCharacterInLine);
            SetPhysicalOffset(PhysicalOffset);
        }

    private:
        int indexOfLine;
        int indexOfCharacterInLine;
        int physicalOffset;
    };
};

#endif // CPPBG_TRA_POSITION