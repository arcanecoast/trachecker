#include "TranslationException.h"

using namespace cppbg_tra;

TranslationException::TranslationException() :
    indexOfBadLine(-1),
    indexOfBadCharacterInLine(-1),
    offsetToBadCharacter(-1)
{
}

TranslationException::TranslationException(int IndexOfBadLine, int IndexOfBadCharacterInLine, int OffsetToBadCharacter) :
    indexOfBadLine(IndexOfBadLine),
    indexOfBadCharacterInLine(IndexOfBadCharacterInLine),
    offsetToBadCharacter(OffsetToBadCharacter)
{
}

TranslationException::TranslationException(const Position& Pos) :
    indexOfBadLine(Pos.GetIndexOfLine()),
    indexOfBadCharacterInLine(Pos.GetPlaceInLine()),
    offsetToBadCharacter(Pos.GetPhysicalOffset())
{
}