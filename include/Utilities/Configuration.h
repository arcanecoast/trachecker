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
 
 This unit contains configuration file description.
 
********************************************************************************************/

#ifndef TRACHECKER_CONFIGURATION
#define TRACHECKER_CONFIGURATION

class Configuration
{
public:
    Configuration();

    void Load();
    void Save() const;

    bool GetAutoRecheckOnSave() const;
    wxString GetDefaultCodepage() const;
    bool GetMaximized() const;
    int  GetHeight() const;
    int  GetWidth() const;

    void SetAutoRecheckOnSave(bool val);
    void SetDefaultCodepage(const wxString& val);
    void SetMaximized(bool val);
    void SetHeight(int val);
    void SetWidth(int val);

private:
    bool m_autoRecheckOnSave;
    wxString m_defaultCodepage;
    bool m_maximized;
    int  m_height;
    int  m_width;

    static const wxString appName;
    static const wxString vendorName;
    static const wxString localFilename;
    static const wxString globalFilename;

    static const bool default_autoRecheckOnSave;
    static const wxString default_defaultCodepage;
    static const bool default_maximized;
    static const int  default_height;
    static const int  default_width;
};

#endif //TRACHECKER_CONFIGURATION