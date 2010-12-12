#pragma once

#include "uEye.h"
#include "ofMain.h"

enum _disp_mode 
{
  e_disp_mode_bitmap = 0,
  e_disp_mode_directdraw
};

class ofxIds {
public:
    ofxIds();
    BOOL OpenCamera();
    BOOL CloseCamera();
    
    int getWidth();
    int getHeight();
    void snapImage(ofImage& img);

protected:
    BOOL            m_boLive;
    BOOL            m_boDisplay;
    HCAM            m_hCam;
    CAMINFO         m_ci;
    SENSORINFO      m_si;
    int             m_nSizeX, m_nSizeY;
    int             m_nDispSizeX, m_nDispSizeY;
    char*           m_pcImageMemory;
    int             m_nMemoryId;
    int             m_nColorMode;
    int             m_nBitsPerPixel;
    int             m_nDispModeSel;
    HWND            m_hwndDisp;
    ATOM            m_atom;
    HANDLE          m_hMsgThread;
    DWORD           m_dwMsgThreadID;
    BOOL            m_boMsgThreadRun;
    HANDLE          m_hevThreadOk;
    
    INT InitCamera (HIDS *hCam, HWND hWnd);
    void GetMaxImageSize(INT *pnSizeX, INT *pnSizeY);
    int InitDisplayMode();
};
