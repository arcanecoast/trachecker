#ifndef _FUNCS_FS_TREE_
#define _FUNCS_FS_TREE_

#include <list>
#include <string>

class FS_Attributes
{
private:
	std::string Name;

public:
	FS_Attributes(const std::string &sName = ""):Name(sName) {}
	
	const std::string &GetName() const { return Name; }
    void SetName(const std::string &sName) { Name = sName; }
};

class FS_File: public FS_Attributes
{
public:
	FS_File(const std::string &sName = ""):FS_Attributes(sName) {}
};

class FS_Folder: public FS_Attributes
{
public:
	std::list<FS_Folder> SubFolders;
	std::list<FS_File>   Files;

private:
    std::string ParentPath;

public:
    FS_Folder(const std::string &sFolderPath);
	FS_Folder(const std::string &sParentPath, const std::string &sName);

    const std::string &GetParentPath() const { return ParentPath; }

private:
	void MakeTree(const std::string &Path);
};

bool HasDifferentContent(FS_Folder &folder1, FS_Folder &folder2);
std::string CompareTranslationFilesInFolders(const char *folderPath1, const char *folderPath2);

#endif //_FUNCS_FS_TREE_