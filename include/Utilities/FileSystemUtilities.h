#ifndef TRACHECKER_FILESYSTEM_UTILITIES
#define TRACHECKER_FILESYSTEM_UTILITIES

#include <set>
#include <wx/string.h>

using std::set;

set<wxString> GetFilesList(const wxString& path);

#endif //TRACHECKER_FILESYSTEM_UTILITIES