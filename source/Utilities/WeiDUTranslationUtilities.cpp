#include <wx/translation.h>

#include <Utilities/WeiDUTranslationUtilities.h>
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

wxString CheckWeiduTranslationToBeTLKCompatible(const WeiDUModTranslation& translation)
{
    wxString errors = "";

    if (translation.Empty()) {
        errors += _("- Translation file is empty.\n");

        return errors;
    }

    int prevId = translation.Begin()->first;

    if (prevId != 0 && prevId != 1) {
        errors += wxString::Format(_("- Translation file's first entry must have @0 or @1 index but first index is @%i.\n"), prevId);
    }

    for (WeiDUModTranslation::const_iterator currentEntry = ++translation.Begin(), entryEnd = translation.End(); currentEntry != entryEnd; prevId = currentEntry->first, ++currentEntry) {
        if (currentEntry->first - prevId != 1) {
            errors += wxString::Format(_("- Translation file has non contiguous entries (@%i, @%i).\n"), prevId, currentEntry->first);

            break;
        }
    }

    return errors;
}
