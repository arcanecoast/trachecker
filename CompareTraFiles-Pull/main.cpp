#include "cppbg/tra/HighLevelParser.h"
#include "fs_funcs.h"

#include <set>
#include <string>

using namespace std;
using namespace cppbg_tra;

bool HasDifferentStructure(FS_Folder &folder1, FS_Folder &folder2)
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
            printf("Folder 2 has 1 more file: %s\n", i2->GetName().c_str());

            hasDifferentStructure = true;
        }
    }

    for (set<string>::iterator spareIter = spareFilesNames.begin(), spareEnd = spareFilesNames.end(); spareIter != spareEnd; ++spareIter) {
        printf("Folder 1 has 1 more file: %s\n", spareIter->c_str());

        hasDifferentStructure = true;
    }

    return hasDifferentStructure;
}

void CompareFolders(const char *folderPath1, const char *folderPath2)
{
    FS_Folder folder1(folderPath1), folder2(folderPath2);

    if (HasDifferentStructure(folder1, folder2)) {
        printf("Folders have different structure! That is no good!\n");
    } else {
        printf("Folders have same structure! That is OK!\n");
    }

    for (list<FS_File>::iterator i1 = folder1.Files.begin(), end1 = folder1.Files.end(); i1 != end1; ++i1) {
        HighLevelParser traFromFolder1, traFromFolder2;

        traFromFolder1.LoadFromFile((string(folderPath1) + "\\" + i1->GetName()).c_str());
        traFromFolder2.LoadFromFile((string(folderPath2) + "\\" + i1->GetName()).c_str());

        if (traFromFolder1.Size() != traFromFolder2.Size()) {
            printf("Size is different for %s\n", i1->GetName().c_str());
        }
    }
}

int main()
{
    CompareFolders(
        "E:\\Downloads\\BG1NPC-21-PR_20141231\\BG1NPC-21-PR_20141231\\bg1npc\\tra\\english",
        "E:\\Downloads\\BG1NPC-21-PR_20141231\\BG1NPC-21-PR_20141231\\bg1npc\\tra\\russian"
    );

    return 0;
}