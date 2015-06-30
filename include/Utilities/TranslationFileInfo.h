/****************************************************************************************//**

 Author of this file is
     __________ _______   __   _____ __ __________ _____ __ __ _____ ______   __ ____   
    / _   _   // ___  /__/ /_ / ___ / // _   _   // ___ / // //____// ____ \ / //___ \  
   / / / / / // /__/_//_  __// /  /  // / / / / // /  /  //   /    / /___/_//   /__/_/  
  / / / / / // /_____  / /_ / /__/  // / / / / // /__/  // /\ \__ / /_____ / /\ \__     
 /_/ /_/ /_/ \______/  \__/ \____/_//_/ /_/ /_/ \____/_//_/  \___/\______//_/  \___/    
                                                                                         
 webpage: http://www.goblinov.net
 email: guru@goblinov.net   

 This file is provided under certain circumstances. For more details see LICENSE file in
 the project's root folder.
  
 \author metamaker
 \brief 
 
 This unit contains class which describes current state of translation file from text editor.
 
********************************************************************************************/

#ifndef TRACHECKER_TRANSLATION_FILE_INFO
#define TRACHECKER_TRANSLATION_FILE_INFO

#include <wx/string.h>

class TranslationFileInfo
{
public:
    TranslationFileInfo();

    wxString GetPath() const;
    bool GetVerified() const;
    bool GetModified() const;
    wxString GetCodepage() const;

    void SetPath(const wxString& path);
    void SetVerified(bool value);
    void SetModified(bool value);
    void SetCodepage(const wxString& codepage);

private:
    wxString m_currentCodePage;
    wxString m_filePath;

    bool m_isFileVerified;  // Current file is checked with TRA HighLevelParser
    bool m_isFileModified; // Current file is modified
};

#endif //TRACHECKER_TRANSLATION_FILE_INFO