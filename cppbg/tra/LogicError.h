#ifndef CPPBG_TRA_LOGICERROR
#define CPPBG_TRA_LOGICERROR

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 */

#include "TranslationException.h"

namespace cppbg_tra
{
    class LogicError: public TranslationException
    {
    public:
        enum ErrorType {
            ERROR_FOUND_CONTENT_NOT_ASSIGNED_TO_ENTRY        = 0,
            ERROR_WRONG_INDEX_FORMAT                         = 1,
            ERROR_NO_TEXT_SECTION_BEFORE_SOUND_SPECIFICATION = 2,
            ERROR_THIRD_TEXT_SECTION                         = 3,
            ERROR_THIRD_SOUND_SPECIFICATION                  = 4,
            ERROR_LINK_POINTS_TO_NOWHERE                     = 5
        };

    public:
        explicit LogicError(ErrorType Type);
        explicit LogicError(ErrorType Type, int IndexOfBadLine, int IndexOfBadCharacterInLine, int OffsetToBadCharacter);
        explicit LogicError(ErrorType Type, const Position& Pos);

        ErrorType GetType() const { return this->type; }
        virtual string GetHint() const
        {
            switch (this->type) {
                case ERROR_FOUND_CONTENT_NOT_ASSIGNED_TO_ENTRY:
                    return "Logic error: content without @id";
                case ERROR_WRONG_INDEX_FORMAT:
                    return "Logic error: bad @id format";
                case ERROR_NO_TEXT_SECTION_BEFORE_SOUND_SPECIFICATION:
                    return "Logic error: no text section before sound specification";
                case ERROR_THIRD_TEXT_SECTION:
                    return "Logic error: third text section detected";
                case ERROR_THIRD_SOUND_SPECIFICATION:
                    return "Logic error: third sound specification detected";
                case ERROR_LINK_POINTS_TO_NOWHERE:
                    return "Logic error: broken link";
                default:
                    return "Unknown logic error";
            }            
        }

    private:
        ErrorType type;
    };
};

#endif // CPPBG_TRA_LOGICERROR