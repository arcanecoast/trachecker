# TRA Checker

Application for checking and editing BG WeiDU's .tra files.
For license limitations see LICENSE.md.

Features:
  - Text files editing;
  - Single and batch .tra syntax checking;
  - Multiple languages support (for program's user interface).

Requirements:
  - wxWidgets 3.0.x;
  - metamaker's cppbg library (https://github.com/metamaker/cppbg).
 
### Version history

**28.06.2015:**

Enhancements: 
  - New checks for .TRA exporting to .TLK are implemented (if .tra is empty, if .tra has non contiguous entries, if .tra first index is not 0 or 1);
  - Application now asks if user really want to reload text file.
    
**04.04.2015:**

New features: 
  - Export/import to .TLK;
  - UTF-8 support;
  - New parameter "DefaultCodepage" in config file which sets default encoding for file saving.

**05.12.2013:**
  - Initial release.