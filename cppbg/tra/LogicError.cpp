#include "LogicError.h"

using namespace cppbg_tra;

LogicError::LogicError(ErrorType Type) :
    type(Type)
{
}

LogicError::LogicError(ErrorType Type, int IndexOfBadLine, int IndexOfBadCharacterInLine, int OffsetToBadCharacter) :
    TranslationException(IndexOfBadLine, IndexOfBadCharacterInLine, OffsetToBadCharacter),
    type(Type)
{
}

LogicError::LogicError(ErrorType Type, const Position& Pos) :
    TranslationException(Pos),
    type(Type)
{
}