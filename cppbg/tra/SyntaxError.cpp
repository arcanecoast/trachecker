#include "SyntaxError.h"

using namespace cppbg_tra;

SyntaxError::SyntaxError(ErrorType Type) :
    type(Type)
{
}

SyntaxError::SyntaxError(ErrorType Type, int IndexOfBadLine, int IndexOfBadCharacterInLine, int OffsetToBadCharacter) :
    TranslationException(IndexOfBadLine, IndexOfBadCharacterInLine, OffsetToBadCharacter),
    type(Type)
{
}

SyntaxError::SyntaxError(ErrorType Type, const Position& Pos) :
    TranslationException(Pos),
    type(Type)
{
}