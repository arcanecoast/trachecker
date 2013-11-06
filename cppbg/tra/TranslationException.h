#ifndef CPPBG_TRA_TRANSLATIONEXCEPTION
#define CPPBG_TRA_TRANSLATIONEXCEPTION

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 */

#include <string>

#include "Position.h"

using std::string;

namespace cppbg_tra
{
    /**
     * Base class for .tra parser's exceptions.
     *
     * @see SyntaxError, LogicError 
     */
    class TranslationException
    {
    public:
        TranslationException();
        explicit TranslationException(int IndexOfBadLine, int IndexOfBadCharacterInLine,
            int OffsetToBadCharacter);
        explicit TranslationException(const Position& Pos);

        virtual ~TranslationException() {}

        /**
         * Get textual description of error.
         *
         * @return Textual hint.
         */
        virtual string GetHint() const { return ""; };

        /**
         * Get index of line where error is happened.
         *
         * @return Index of line with error.
         */
        int GetIndexOfBadLine() const { return this->indexOfBadLine; }

        /**
         * Get index of character in line where error is happened.
         *
         * @return Index of character in line with error.
         */
        int GetIndexOfBadCharacterInLine() const { return this->indexOfBadCharacterInLine; }

        /**
         * Get offset from start of file to character where error is happened.
         *
         * @return Offset from start of file to bad character.
         */
        int GetOffsetToBadCharacter() const { return this->offsetToBadCharacter; }

    private:
        int indexOfBadLine;
        int indexOfBadCharacterInLine;
        int offsetToBadCharacter;
    };
};

#endif // CPPBG_TRA_TRANSLATIONEXCEPTION