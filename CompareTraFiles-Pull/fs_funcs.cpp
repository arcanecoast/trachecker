#include "fs_funcs.h"
#include <io.h>

#include <string>

using namespace std;

FS_Folder::FS_Folder(const std::string &sParentPath, const std::string &sName) :
    FS_Attributes(sName), ParentPath(sParentPath)
{
	if (!sParentPath.empty() && !sName.empty()) MakeTree(sParentPath + "\\" + sName);
}

FS_Folder::FS_Folder(const std::string &sFolderPath)
{
    string::size_type lastSlashPos = sFolderPath.rfind('\\');

    if (lastSlashPos != string::npos) {
        ParentPath = sFolderPath.substr(0, lastSlashPos);
        string sName = sFolderPath.substr(lastSlashPos + 1);

        this->SetName(sName);

        if (!ParentPath.empty() && !sName.empty()) MakeTree(ParentPath + "\\" + sName);
    }
}

void FS_Folder::MakeTree(const std::string &Path)
{
	_finddata_t fd;
	intptr_t iPtr = _findfirst((Path + "\\*").c_str(), &fd);
	if (iPtr == -1) return;
	do
	{
		const std::string name = fd.name;
		if ((fd.attrib & _A_SUBDIR) && name != "." && name != "..")
		{			 
			SubFolders.push_back(FS_Folder(Path, name));	 
		}
		else if (name != "." && name != "..")
		{
			Files.push_back(FS_File(name));
		}
	}  while (_findnext(iPtr, &fd) == 0);
	_findclose(iPtr);
}