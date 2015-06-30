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
 
 This units contains actions which can be performed on translation file from text editor.
 
********************************************************************************************/

#ifndef TRACHECKER_TRANSLATION_FILE_ACTIONS
#define TRACHECKER_TRANSLATION_FILE_ACTIONS

#include <wx/wx.h>

class wxStyledTextCtrl;

namespace TranslationFileActions {
    void LoadTranslationFromFile(const wxString& filepath, wxStyledTextCtrl* textEditor);
    void SaveTranslationToFile(const wxString& filepath, const wxString& encoding, wxStyledTextCtrl* textEditor);

    //! Create temporary file and copy text editor contents to it.
    /*!
    \param textEditor text editor to copy content from.
    \return Path to temporary file in case of success or "" if function has been failed.
    */
    wxString CreateTemporaryFileWithTextEditorContent(wxStyledTextCtrl* textEditor);
}

#endif //TRACHECKER_TRANSLATION_FILE_ACTIONS