
#ifndef MESSAGEPROCESSOR_HPP
#define MESSAGEPROCESSOR_HPP

//#define STATE_SYSTEM_INVISIBLE  = 0x00008000

#include <stdio.h> 
#include <stdlib.h> 
#include <windows.h> 
#include <commctrl.h>
#include <winuser.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/filesystem.hpp>

#ifndef STATE_SYSTEM_INVISIBLE
	#define STATE_SYSTEM_INVISIBLE 0x00008000 
#endif

//#include <mfidl.h>
//#define _WIN32_WINNT 0x0601
//#define WINVER 0x0601
//#include <Devicetopology.h>
//#include <audioclient.h>
//#include <mmdeviceapi.h>
//#include <Endpointvolume.h>
//#include <Ksmedia.h>
//#include <mmdeviceapi.h>

BOOL IsAltTabWindow2(HWND hwnd){
  TITLEBARINFO ti;
  HWND hwndTry, hwndWalk = NULL;
    
  if(!IsWindowVisible(hwnd))
    return FALSE;

  hwndTry = GetAncestor(hwnd, GA_ROOTOWNER);

  while(hwndTry != hwndWalk) {
    hwndWalk = hwndTry;
    hwndTry = GetLastActivePopup(hwndWalk);
    if(IsWindowVisible(hwndTry)) 
      break;
  }

  if(hwndWalk != hwnd)
    return FALSE;

  // the following removes some task tray programs and "Program Manager"
  ti.cbSize = sizeof(ti);
  GetTitleBarInfo(hwnd, &ti);
  if(ti.rgstate[0] & STATE_SYSTEM_INVISIBLE)
    return FALSE;

  // Tool windows should not be displayed either, these do not appear in the
  // task bar.
  if(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
    return FALSE;

  return TRUE;
}


BOOL CALLBACK MyEnumWindowsProc(HWND hwnd, LPARAM lparam){
  //-- int& i = *(reinterpret_cast<int*>(lparam));
  //std::cerr << "MyEnumWindowsProc\n";
  std::string& str = *(reinterpret_cast<std::string*>(lparam));
  //std::cerr << "MyEnumWindowsProc2\n";
  //++i;
  char title[256];
  if (IsAltTabWindow2(hwnd)){
    ::GetWindowText(hwnd, title, sizeof(title));
    //std::cerr << str << std::endl;
    str += title;
    str += " | ";
    DWORD dwThreadID;
    GetWindowThreadProcessId(hwnd, &dwThreadID);
    HANDLE phwnd = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwThreadID);
    //printf("Openproc Error:%d \n",GetLastError());
    char buffer[256] = "";
    DWORD mfln = GetProcessImageFileNameA(phwnd, buffer, 256);
    str += buffer;
    str += "\n\r";
    //DWORD mfln = GetModuleFileNameEx(phwnd,NULL, buffer, 256);
    //::printf("    - Window  (%x): %s \n        Process:%s \n",  hwnd, title, buffer);
    //printf(str.c_str());
    //printf("Error:%d \n",GetLastError());
  }
  return TRUE;
}


class messageprocessor {


public:

  //-- constructeur
  messageprocessor() {
  }

  std::string listwindows(){
    int i = 0;
    std::string str;
    ::EnumWindows(&MyEnumWindowsProc, reinterpret_cast<LPARAM>(&str));
    return str;
  }

  void enterswitchmod(){
    //-- sending keys
    keybd_event(VK_MENU,0xb8,0 , 0);
    keybd_event(VK_TAB,0x8f,0 , 0);
    keybd_event(VK_TAB,0x8f, KEYEVENTF_KEYUP,0);
    keybd_event(VK_SHIFT,0x8f, 0,0);
    keybd_event(VK_TAB,0x8f, 0,0);
    keybd_event(VK_SHIFT,0x8f, KEYEVENTF_KEYUP,0);
  }

  void quitswitchmod(){
    keybd_event(VK_MENU,0xb8, KEYEVENTF_KEYUP, 0);
  }

  void switchright(){
    keybd_event(VK_TAB,0xb8, 0, 0);
    keybd_event(VK_TAB,0x8f, KEYEVENTF_KEYUP,0);
  }

  void switchleft(){
    keybd_event(VK_SHIFT,0x8f, 0,0);
    keybd_event(VK_TAB,0xb8, 0, 0);
    keybd_event(VK_TAB,0x8f, KEYEVENTF_KEYUP,0);
    keybd_event(VK_SHIFT,0x8f, KEYEVENTF_KEYUP,0);
  }

  void puttosleep(){
    //SetSuspendState(false, false, false);
    SetSystemPowerState(false,0);
  }

  void volumeup(){    
    keybd_event(VK_VOLUME_UP,0x8f, 0,0);
    keybd_event(VK_VOLUME_UP,0x8f, KEYEVENTF_KEYUP,0);
  }

  void volumedown(){    
    keybd_event(VK_VOLUME_DOWN,0x8f, 0,0);
    keybd_event(VK_VOLUME_DOWN,0x8f, KEYEVENTF_KEYUP,0);
  }

  bool shutdown(){
    //ExitWindowsEx(EWX_POWEROFF | EWX_FORCEIFHUNG,SHTDN_REASON_MINOR_OTHER);
    HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 
 
    // Get a token for this process. 
 
    if (!OpenProcessToken(GetCurrentProcess(), 
			  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
      return( FALSE ); 
 
    // Get the LUID for the shutdown privilege. 
 
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
			 &tkp.Privileges[0].Luid); 
 
    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
    // Get the shutdown privilege for this process. 
 
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
			  (PTOKEN_PRIVILEGES)NULL, 0); 
 
    if (GetLastError() != ERROR_SUCCESS) 
      return FALSE; 
 
    // Shut down the system and force all applications to close. 
 
    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 
               SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
               SHTDN_REASON_MINOR_UPGRADE |
		       SHTDN_REASON_FLAG_PLANNED)) 
      return FALSE; 

    //shutdown was successful
    return TRUE;
  }
  
  //-- listing du repertoir fourtni en parametre
  std::string listfiles(std::string sp){
    using namespace std;
    using namespace boost::filesystem;

    ostringstream result ;

    try{

      if (exists(sp)){
	if (is_regular_file(sp))        // is p a regular file?   
	  cout << sp << " size is " << file_size(sp) << '\n';
	
	else if (is_directory(sp)){
	  cout << sp << " is a directory containing:\n";
	  copy(directory_iterator(sp), directory_iterator(), // directory_iterator::value_type
	       ostream_iterator<directory_entry>(cout, "\n")); // is directory_entry, which is
	  copy(directory_iterator(sp), directory_iterator(), // directory_iterator::value_type
	       ostream_iterator<directory_entry>(result, "|")); // is directory_entry, which is

	}else
	  cout << sp << " exists, but is neither a regular file nor a directory\n";
      }else
	cout << sp << " does not exist\n";
    }

    catch (const filesystem_error& ex){
      cout << ex.what() << '\n';
    }
    return result.str();
  }
};

#endif // MESSAGE_HPP
