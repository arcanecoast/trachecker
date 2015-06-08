#include <string>
#include <set>
#include <io.h>

#include <Utilities/FileSystemUtilities.h>
#include <cppbg/tra/WeiDUModTranslation.h>

using namespace std;
using cppbg_tra::WeiDUModTranslation;

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

bool HasDifferentContent(FS_Folder &folder1, FS_Folder &folder2)
{
    bool hasDifferentStructure = false;

    set<string> spareFilesNames;
    set<string> containingFilesNames;

    for (list<FS_File>::iterator i1 = folder1.Files.begin(), end1 = folder1.Files.end(); i1 != end1; ++i1) {
        spareFilesNames.insert(i1->GetName());
    }

    for (list<FS_File>::iterator i2 = folder2.Files.begin(), end2 = folder2.Files.end(); i2 != end2; ++i2) {
        if (spareFilesNames.find(i2->GetName()) != spareFilesNames.end()) {
            spareFilesNames.erase(i2->GetName());
            containingFilesNames.insert(i2->GetName());
        } else {
            // printf("Folder 2 has 1 more file: %s\n", i2->GetName().c_str());

            hasDifferentStructure = true;
        }
    }

    for (set<string>::iterator spareIter = spareFilesNames.begin(), spareEnd = spareFilesNames.end(); spareIter != spareEnd; ++spareIter) {
        // printf("Folder 1 has 1 more file: %s\n", spareIter->c_str());

        hasDifferentStructure = true;
    }

    return hasDifferentStructure;
}

std::string CompareTranslationFilesInFolders(const char *folderPath1, const char *folderPath2)
{
    string result;
    FS_Folder folder1(folderPath1), folder2(folderPath2);

    if (HasDifferentContent(folder1, folder2)) {
        result += "Folders have different content! That is no good!\n";
    } else {
        result += "Folders have same structure! That is OK!\n";
    }

    for (list<FS_File>::iterator i1 = folder1.Files.begin(), end1 = folder1.Files.end(); i1 != end1; ++i1) {
        WeiDUModTranslation traFromFolder1, traFromFolder2;

        traFromFolder1.LoadFromFile((string(folderPath1) + "\\" + i1->GetName()).c_str());
        traFromFolder2.LoadFromFile((string(folderPath2) + "\\" + i1->GetName()).c_str());

        if (traFromFolder1.Size() != traFromFolder2.Size()) {
            result += "Size is different for ";
            result += i1->GetName().c_str();
            result += "\n";
        }
    }

    return result;
}

