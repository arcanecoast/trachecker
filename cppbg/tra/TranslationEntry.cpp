#include "TranslationEntry.h"
#include "TranslationEntryUtilities.h"

using namespace cppbg_tra;

TranslationEntry::TranslationEntry(int ID, const string& MainText, const string& MainSound,
	const string& SecondaryText, const string& SecondarySound) :
	linkedEntry(static_cast<const TranslationEntry*>(0)),
	mainText(MainText),
	mainSound(MainSound),
	secondaryText(SecondaryText),
	secondarySound(SecondarySound),
	id(ID)
{
}

TranslationEntry::TranslationEntry(int ID, shared_ptr<const TranslationEntry> LinkedEntry) :
	linkedEntry(LinkedEntry),
	id(ID)
{
}

string TranslationEntry::GetMainText() const
{
	if (this->linkedEntry.get()) {
		return this->linkedEntry->GetMainText();
	} else {
		return this->mainText;
	}
}

string TranslationEntry::GetMainSound() const
{
	if (this->linkedEntry.get()) {
		return this->linkedEntry->GetMainSound();
	} else {
		return this->mainSound;
	}
}

string TranslationEntry::GetSecondaryText() const 
{
	if (this->linkedEntry.get()) {
		return this->linkedEntry->GetSecondaryText();
	} else {
		return this->secondaryText;
	}
}

string TranslationEntry::GetSecondarySound() const
{
	if (this->linkedEntry.get()) {
		return this->linkedEntry->GetSecondarySound();
	} else {
		return this->secondarySound;
	}
}

string TranslationEntry::GetWritableData() const
{
    char idval[16];
    string result = "@";

    sprintf(idval, "%i", this->id);
    result += idval;
    result += " = ";

    if (this->linkedEntry.get()) {
        result += "@";
        sprintf(idval, "%i", this->linkedEntry->id);
        result += idval;
    } else {
        result += escape_content(GetMainText());
        if (!GetMainSound().empty()) {
            result += " [" + GetMainSound() + ']';
        }

        if (!GetSecondaryText().empty()) {
            result += ' ' + escape_content(GetSecondaryText());

            if (!GetSecondarySound().empty()) {
                result += " [" + GetSecondarySound() + ']';
            }
        }
    }

    return result;
}

void TranslationEntry::SetMainText(const string& Text)
{
	Detach();
	this->mainText = Text;
}

void TranslationEntry::SetMainSound(const string& Sound)
{
	Detach();
	this->mainSound = Sound;
}

void TranslationEntry::SetSecondaryText(const string& Text)
{
	Detach();
	this->secondaryText = Text;
}

void TranslationEntry::SetSecondarySound(const string& Sound)
{
	Detach();
	this->secondarySound = Sound;
}

void TranslationEntry::AttachToOtherEntry(shared_ptr<const TranslationEntry> LinkedEntry)
{
	if (LinkedEntry.get() == 0) {
		Detach();
	} else {
		this->linkedEntry = LinkedEntry;
	}
}

void TranslationEntry::Detach()
{
	if (this->linkedEntry.get()) {
		this->mainText = this->linkedEntry->GetMainText();
		this->mainSound = this->linkedEntry->GetMainSound();
		this->secondaryText = this->linkedEntry->GetSecondaryText();
		this->secondarySound = this->linkedEntry->GetSecondarySound();

		this->linkedEntry.reset(static_cast<const TranslationEntry*>(0));
	}
}