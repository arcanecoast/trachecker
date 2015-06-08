#include <Utilities/WeiDUTranslationToTalkTable.h>
#include <cppbg/tra/TranslationEntryUtilities.h>

using cppbg_tra::EscapeTranslationContent;

wxString TalkTableEntryToString(const TalkTableEntry& entry, const size_t& index)
{
    wxString result = wxString::Format("@%i = %s", index, EscapeTranslationContent(entry.GetText()));

    if (!entry.GetSoundResRef().empty()) {
        result += " [" + entry.GetSoundResRef() + "]";
    }

    return result;
}
