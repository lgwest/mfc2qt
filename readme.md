https://docs.huihoo.com/qt/solutions/4/qtwinmigrate/winmigrate-walkthrough.html 

- next step(2): replace the about dialog  
  _Ring.cpp .h .rc_ and _resource.h_
 
    ~~~
    resource.h:5:#define IDD_ABOUTBOX                    100
    Ring.cpp:21:    ON_COMMAND(ID_APP_ABOUT, &CRingApp::OnAppAbout)
    Ring.cpp:126:// CAboutDlg dialog used for App About
    Ring.cpp:128:class CAboutDlg : public CDialog
    Ring.cpp:131:   CAboutDlg();
    Ring.cpp:134:   enum { IDD = IDD_ABOUTBOX };
    Ring.cpp:144:CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
    Ring.cpp:148:void CAboutDlg::DoDataExchange(CDataExchange* pDX)
    Ring.cpp:153:BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    Ring.cpp:157:void CRingApp::OnAppAbout()
    Ring.cpp:159:   CAboutDlg aboutDlg;
    Ring.cpp:160:   aboutDlg.DoModal();
    Ring.h:28:      afx_msg void OnAppAbout();
    Ring.rc:61:    BUTTON      ID_APP_ABOUT
    Ring.rc:91:        MENUITEM "&About Ring...",              ID_APP_ABOUT
    Ring.rc:143:        MENUITEM "&About Ring...",              ID_APP_ABOUT
    Ring.rc:180:IDD_ABOUTBOX DIALOGEX 0, 0, 235, 55
    Ring.rc:182:CAPTION "About Ring"
    Ring.rc:239:    IDD_ABOUTBOX, DIALOG
    Ring.rc:263:    ID_APP_ABOUT            "Display program information, version number and copyright\nAbout"
    ~~~
