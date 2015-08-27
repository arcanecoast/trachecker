#include <io.h>

#include <Utilities/FileSystemUtilities.h>

using namespace std;

set<wxString> GetFilesList(const wxString& path)
{
	set<wxString> files;

	_finddata_t fd;
	intptr_t iPtr = _findfirst((path + "\\*").c_str(), &fd);

	if (iPtr != -1) {
		do {
			const wxString name = fd.name;

			if ((fd.attrib & _A_SUBDIR) && name != "." && name != "..") {
				// subFolders.push_back(Folder(path + "\\" + name));

				// Do nothing
			}
			else if (name != "." && name != "..") {
				files.insert(name);
			}
		} while (_findnext(iPtr, &fd) == 0);

		_findclose(iPtr);
	}

	return files;
}
