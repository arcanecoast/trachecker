#include <GUI/DialogCompareFolders.h>
#include <GUI/WindowMain.h>
#include <GUI/Application.h>

#include <wx/dirdlg.h>

#include <Utilities/FileSystemUtilities.h>
#include <cppbg/tra/WeiDUModTranslation.h>

enum 
{
    ID_CLOSE = 2000,
    ID_COMPARE_BY_CONTENT,
	ID_COMPARE_BY_COUNT,

	ID_SELECT_FIRST_FOLDER,
	ID_SELECT_SECOND_FOLDER,

    ID_FILES_LIST
};

wxBEGIN_EVENT_TABLE(CompareFoldersDialog, wxDialog)
	EVT_BUTTON(ID_CLOSE, CompareFoldersDialog::OnCloseClick)
	EVT_BUTTON(ID_COMPARE_BY_CONTENT, CompareFoldersDialog::OnCompareByEntriesContentClick)
	EVT_BUTTON(ID_COMPARE_BY_COUNT, CompareFoldersDialog::OnCompareByEntriesCountClick)
	EVT_BUTTON(ID_SELECT_FIRST_FOLDER, CompareFoldersDialog::OnSelectFirstFolderClick)
	EVT_BUTTON(ID_SELECT_SECOND_FOLDER, CompareFoldersDialog::OnSelectSecondFolderClick)
wxEND_EVENT_TABLE()

using std::set;
using cppbg_tra::WeiDUModTranslation;

CompareFoldersDialog::CompareFoldersDialog(MainWindow *parent) :
    wxDialog(parent, wxID_ANY, _("Compare folders with translations"),
        wxDefaultPosition, wxSize(600, 400),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU | wxRESIZE_BORDER)
{
	wxBoxSizer *sizerOptions = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *sizerFirstFolderLine = new wxBoxSizer(wxHORIZONTAL);
	m_editFirstFolder = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NOHIDESEL);
	sizerFirstFolderLine->Add(m_editFirstFolder, 1, wxEXPAND | wxALL, 2);
	sizerFirstFolderLine->Add(
		new wxButton(this, ID_SELECT_FIRST_FOLDER, _("Select..."), wxDefaultPosition, wxDefaultSize),
		0, wxALL, 2
	);

	wxBoxSizer *sizerSecondFolderLine = new wxBoxSizer(wxHORIZONTAL);
	m_editSecondFolder = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NOHIDESEL);
	sizerSecondFolderLine->Add(m_editSecondFolder, 1, wxEXPAND | wxALL, 2);
	sizerSecondFolderLine->Add(
		new wxButton(this, ID_SELECT_SECOND_FOLDER, _("Select..."), wxDefaultPosition, wxDefaultSize),
		0, wxALL, 2
	);

	wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonsSizer->Add(
		new wxButton(this, ID_COMPARE_BY_COUNT, _("Compare by entries count"), wxDefaultPosition, wxDefaultSize),
		1, wxEXPAND | wxALL, 2
	);
	buttonsSizer->Add(
		new wxButton(this, ID_COMPARE_BY_CONTENT, _("Compare by entries content"), wxDefaultPosition, wxDefaultSize),
		1, wxEXPAND | wxALL, 2
	);

	sizerOptions->Add(new wxStaticText(this, wxID_ANY, _("First folder:"), wxDefaultPosition, wxDefaultSize), 0, wxEXPAND | wxALL, 2);
	sizerOptions->Add(sizerFirstFolderLine, 1, wxEXPAND | wxALL, 2);
	sizerOptions->Add(new wxStaticText(this, wxID_ANY, _("Second folder:"), wxDefaultPosition, wxDefaultSize), 0, wxEXPAND | wxALL, 2);
	sizerOptions->Add(sizerSecondFolderLine, 1, wxEXPAND | wxALL, 2);
	sizerOptions->Add(buttonsSizer, 1, wxEXPAND | wxALL, 2);
	
	m_listFiles = new wxListCtrl(this, ID_FILES_LIST, wxDefaultPosition, wxDefaultSize, wxLC_VRULES | wxLC_HRULES | wxLC_SINGLE_SEL | wxLC_REPORT);
    m_listFiles->InsertColumn(0, _("Filename"), wxLIST_FORMAT_LEFT, 200);
    m_listFiles->InsertColumn(1, _("Error"), wxLIST_FORMAT_LEFT, 350);

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(sizerOptions, 0, wxEXPAND | wxALL, 2);
    mainSizer->Add(m_listFiles, 1, wxEXPAND | wxALL, 2);

    wxBoxSizer *bottomButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	bottomButtonsSizer->Add(
		new wxButton(this, ID_CLOSE, _("Close"), wxDefaultPosition, wxSize(100, 25)),
		0, wxLEFT, 2
	);
    mainSizer->Add(bottomButtonsSizer, 0, wxALIGN_RIGHT | wxALL, 2);

    SetSizer(mainSizer);
}

void CompareFoldersDialog::SetFileInfoInList(const wxString& Filename, const wxString& Error)
{
	long itemId = m_listFiles->FindItem(-1, Filename);

	if (itemId == wxNOT_FOUND) {
		itemId = m_listFiles->InsertItem(0, Filename);
	}

	m_listFiles->SetItem(itemId, 1, Error);
}

void CompareFoldersDialog::ClearFilesList()
{
    m_listFiles->DeleteAllItems();
}

void CompareFoldersDialog::OnCloseClick(wxCommandEvent& event)
{
    Close();
}

wxString CompareFoldersDialog::CallSelectFolderDialog(const wxString& defaultFolder)
{
	wxDirDialog DirectorySelector(this, _("Select folder with translation"), defaultFolder, wxDD_DIR_MUST_EXIST | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (DirectorySelector.ShowModal() == wxID_CANCEL) {
		return wxEmptyString;
	}

	return DirectorySelector.GetPath();
}

bool CompareFoldersDialog::HaveDifferentFolderStructure(const set<wxString>& firstFolderFiles, const set<wxString>& secondFolderFiles)
{
	bool haveDifferentStructure = false;

	set<wxString>::const_iterator i1 = firstFolderFiles.begin();
	set<wxString>::const_iterator i2 = secondFolderFiles.begin();

	while (i2 != secondFolderFiles.end()) {
		if (*i1 == *i2) {
			// Filenames are equal

			SetFileInfoInList(*i1, _("Not checked"));

			++i1;
			++i2;
		}
		else if (*i1 < *i2) {
			// In lexicographical order, i1 goes before i2

			SetFileInfoInList(*i1, _("Second folder doesn't contain this file"));
			haveDifferentStructure = true;

			++i1;
		}
		else {
			// In lexicographical order, i1 goes after i2

			SetFileInfoInList(*i2, _("First folder doesn't contain this file"));
			haveDifferentStructure = true;

			++i2;
		}
	}

	return haveDifferentStructure;
}

unsigned int CompareFoldersDialog::CompareFiles(const set<wxString>& filenames, bool compareContent)
{
	unsigned int errorsCount = 0;

	for (set<wxString>::const_iterator filename = filenames.begin(); filename != filenames.end(); ++filename) {
		WeiDUModTranslation firstTranslationFile;
		WeiDUModTranslation secondTranslationFile;

		try {
			firstTranslationFile.LoadFromFile(m_editFirstFolder->GetValue() + "\\" + *filename);
			secondTranslationFile.LoadFromFile(m_editSecondFolder->GetValue() + "\\" + *filename);
		} catch (...) {
			SetFileInfoInList(*filename, _("Unable to load file as .TRA"));
			continue;
		}

		if (firstTranslationFile.Size() > secondTranslationFile.Size()) {
			SetFileInfoInList(*filename, _("File in first folder has more entries than in second"));

			++errorsCount;
		} else if (firstTranslationFile.Size() < secondTranslationFile.Size()) {
			SetFileInfoInList(*filename, _("File in second folder has more entries than in first"));

			++errorsCount;
		} else if (compareContent) {
			WeiDUModTranslation::const_iterator iteratorFirstFile = firstTranslationFile.Begin();
			WeiDUModTranslation::const_iterator iteratorFirstFileEnd = firstTranslationFile.End();

			WeiDUModTranslation::const_iterator iteratorSecondFile = secondTranslationFile.Begin();
			WeiDUModTranslation::const_iterator iteratorSecondFileEnd = secondTranslationFile.End();

			wxString entriesWithDifference = "";

			while (iteratorFirstFile != iteratorFirstFileEnd) {
				if (iteratorFirstFile->second->GetWritableData() != iteratorSecondFile->second->GetWritableData()) {
					++errorsCount;

					entriesWithDifference += wxString::Format(", %i", iteratorFirstFile->first);
				}

				++iteratorFirstFile;
				++iteratorSecondFile;
			}

			if (!entriesWithDifference.empty()) {
				entriesWithDifference = entriesWithDifference.substr(2);

				SetFileInfoInList(*filename, wxString::Format(_("Different entries are %s"), entriesWithDifference));
			} else {
				SetFileInfoInList(*filename, _("No errors"));
			}
		} else {
			SetFileInfoInList(*filename, _("No errors"));
		}
	}

	return errorsCount;
}

void CompareFoldersDialog::OnCompareByEntriesContentClick(wxCommandEvent & event)
{
	ClearFilesList();

	set<wxString>&& firstFolderFiles = GetFilesList(m_editFirstFolder->GetValue());
	set<wxString>&& secondFolderFiles = GetFilesList(m_editSecondFolder->GetValue());

	if (HaveDifferentFolderStructure(firstFolderFiles, secondFolderFiles)) {
		return;
	}

	CompareFiles(firstFolderFiles, true);
}

void CompareFoldersDialog::OnCompareByEntriesCountClick(wxCommandEvent & event)
{
	ClearFilesList();

	set<wxString>&& firstFolderFiles  = GetFilesList(m_editFirstFolder->GetValue());
	set<wxString>&& secondFolderFiles = GetFilesList(m_editSecondFolder->GetValue());

	if (HaveDifferentFolderStructure(firstFolderFiles, secondFolderFiles)) {
		return;
	}

	CompareFiles(firstFolderFiles, false);
}

void CompareFoldersDialog::OnSelectFirstFolderClick(wxCommandEvent & event)
{
	wxString newFolder = CallSelectFolderDialog(m_editFirstFolder->GetValue());

	if (newFolder.empty()) {
		return;
	}

	m_editFirstFolder->SetValue(newFolder);
}

void CompareFoldersDialog::OnSelectSecondFolderClick(wxCommandEvent & event)
{
	wxString newFolder = CallSelectFolderDialog(m_editSecondFolder->GetValue());

	if (newFolder.empty()) {
		return;
	}

	m_editSecondFolder->SetValue(newFolder);
}
