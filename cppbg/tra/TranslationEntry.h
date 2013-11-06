#ifndef CPPBG_TRA_TRANSLATIONENTRY 
#define CPPBG_TRA_TRANSLATIONENTRY

/**
 * Author: metamaker
 * Creation date: 22.08.2013 14:05
 */

#include <string>
#include <boost/shared_ptr.hpp>

using std::string;
using boost::shared_ptr;

namespace cppbg_tra
{
	/**
	 * This class describes entry from WeiDU .tra file.
	 */
	class TranslationEntry
	{
	public:
		explicit TranslationEntry(int ID, const string& MainText, const string& MainSound = "",
			const string& SecondaryText = "", const string& SecondarySound = "");
		explicit TranslationEntry(int ID, shared_ptr<const TranslationEntry> LinkedEntry);

		bool IsAttachedToOtherEntry() const { return this->linkedEntry.get() != 0; }
		shared_ptr<const TranslationEntry> GetAttachedEntry() const { return this->linkedEntry; }

		inline string GetMainText() const;
		inline string GetMainSound() const;
		inline string GetSecondaryText() const;
		inline string GetSecondarySound() const;
		int GetId() const { return this->id; }

        string GetWritableData() const;

		inline void AttachToOtherEntry(shared_ptr<const TranslationEntry> LinkedEntry);
		void Detach();

		inline void SetMainText(const string& Text);
		inline void SetMainSound(const string& Sound);
		inline void SetSecondaryText(const string& Text);
		inline void SetSecondarySound(const string& Sound);
		void SetId(int ID) { this->id = ID; }

	private:
		shared_ptr<const TranslationEntry> linkedEntry;

		string mainText;
		string mainSound;

		string secondaryText;
		string secondarySound;

		int id;
	};
};

#endif // CPPBG_TRA_TRANSLATIONENTRY