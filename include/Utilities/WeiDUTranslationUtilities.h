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
 
 Functions to perform various useful operations on WeiDU .tra files
 
********************************************************************************************/

#ifndef TRACHECKER_WEIDU_TRANSLATION_UTILITIES
#define TRACHECKER_WEIDU_TRANSLATION_UTILITIES

#include <wx/string.h>

#include <cppbg/tlk_v1/TalkTableEntry.h>
#include <cppbg/tra/WeiDUModTranslation.h>

using cppbg_tlk_v1::TalkTableEntry;
using cppbg_tra::WeiDUModTranslation;

wxString TalkTableEntryToString(const TalkTableEntry& entry, const size_t& index);
wxString CheckWeiduTranslationToBeTLKCompatible(const WeiDUModTranslation& translation);

#endif //TRACHECKER_WEIDU_TRANSLATION_TO_TALKTABLE