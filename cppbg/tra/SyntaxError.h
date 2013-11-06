#ifndef CPPBG_TRA_SYNTAXERROR
#define CPPBG_TRA_SYNTAXERROR

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 */

#include "TranslationException.h"

namespace cppbg_tra
{
    class SyntaxError: public TranslationException
	{
    public:
        enum ErrorType {
            ERROR_BAD_INDEX                    = 0,
            ERROR_UNEXPECTED_SYMBOL            = 1,
            ERROR_EQUALENESS_SYMBOL_IS_MISSING = 2,
            ERROR_UNEXPECTED_END_OF_FILE       = 3
        };

	public:
		explicit SyntaxError(ErrorType Type);
		explicit SyntaxError(ErrorType Type, int IndexOfBadLine, int IndexOfBadCharacterInLine, int OffsetToBadCharacter);
        explicit SyntaxError(ErrorType Type, const Position& Pos);

		ErrorType GetType() const { return this->type; }
        virtual string GetHint() const
        {
            switch (this->type) {
                case ERROR_BAD_INDEX:
                    return "Syntax error: bad index";
                case ERROR_UNEXPECTED_SYMBOL:
                    return "Syntax error: unexpected symbol";
                case ERROR_EQUALENESS_SYMBOL_IS_MISSING:
                    return "Syntax error: equaleness symbol is missing";
                case ERROR_UNEXPECTED_END_OF_FILE:
                    return "Syntax error: unexpected end of file";
                default:
                    return "Unknown syntax error";
            }
        }

	private:
		ErrorType type;
	};
};

#endif // CPPBG_TRA_SYNTAXERROR