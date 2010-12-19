#include "ofxIds.h"

ofxIds::ofxIds() {
    m_hCam = NULL;
    m_hwndDisp = NULL;
}

void ofxIds::snapImage(ofImage& img) {
    // set software trigger mode
    is_SetExternalTrigger(m_hCam, IS_SET_TRIGGER_SOFTWARE);

    // capture a single frame. IS_WAIT is required for uEye XS
    ofLog(OF_LOG_VERBOSE, "is_FreezeVideo()");
    is_FreezeVideo(m_hCam, IS_WAIT);

    img.allocate(m_nSizeX, m_nSizeY, OF_IMAGE_COLOR);
    int n = m_nSizeX * m_nSizeY;

    int rawBpp = 4;
    int rawCur = 0;
    unsigned char* rawPixels = (unsigned char*) m_pcImageMemory;

    int imgBpp = 3;
    int imgCur = 0;
    unsigned char* imgPixels = img.getPixels();

    for(int i = 0; i < n; i++) {
        imgPixels[imgCur + 0] = rawPixels[rawCur + 0];
        imgPixels[imgCur + 1] = rawPixels[rawCur + 1];
        imgPixels[imgCur + 2] = rawPixels[rawCur + 2];
        imgCur += imgBpp;
        rawCur += rawBpp;
    }
}

int ofxIds::getWidth() {
    return m_nSizeX;
}

int ofxIds::getHeight() {
    return m_nSizeY;
}

BOOL ofxIds::OpenCamera()
{
    int nRet;

    // init camera
    m_hCam = (HCAM)0;                           // open next camera
    nRet = InitCamera (&m_hCam, m_hwndDisp);    // init camera
    if( nRet == IS_SUCCESS )
    {
        // set software trigger mode
        is_SetExternalTrigger(m_hCam, IS_SET_TRIGGER_SOFTWARE);

        is_GetCameraInfo( m_hCam, &m_ci);

        // retrieve original image size
        is_GetSensorInfo( m_hCam, &m_si);

        GetMaxImageSize(&m_nSizeX, &m_nSizeY);

        /*
        // calc the display size so that it is a integer fraction
        // of the sensor size and max 640 by 480
        int n = 1;
        do
        {
            m_nDispSizeX = m_nSizeX / n;
            n++;
        }
        while(m_nDispSizeX > 640);

        m_nDispSizeY = m_nSizeY / (n - 1);
        */

        // setup the bitmap or directdraw display mode
        nRet = InitDisplayMode();

        if(nRet == IS_SUCCESS)
        {
            // Enable Messages
            nRet = is_EnableMessage( m_hCam, IS_DEVICE_REMOVED, m_hwndDisp );
            nRet = is_EnableMessage( m_hCam, IS_DEVICE_RECONNECTED, m_hwndDisp );
            nRet = is_EnableMessage( m_hCam, IS_FRAME, m_hwndDisp );

            // do the whitebalance once on the first acquisitioned image only on color cameras
            if( m_si.nColorMode == IS_COLORMODE_BAYER )
                nRet = is_SetWhiteBalance( m_hCam, IS_SET_WB_AUTO_ENABLE_ONCE);

            cout << "IDS: capturing at " << m_nSizeX << "x" << m_nSizeY << endl;
        }   // end if( nRet == IS_SUCCESS )
    }   // end if( nRet == IS_SUCCESS )

    return (nRet == IS_SUCCESS);
}


BOOL ofxIds::CloseCamera()
{
    BOOL boRet = FALSE;

    if( m_hCam != 0 )
    {
        is_EnableMessage( m_hCam, IS_FRAME, NULL );
        is_StopLiveVideo( m_hCam, IS_WAIT );
        if( m_pcImageMemory != NULL )
            is_FreeImageMem( m_hCam, m_pcImageMemory, m_nMemoryId );
        m_pcImageMemory = NULL;
        is_ExitCamera( m_hCam );
        m_hCam = NULL;
        boRet = TRUE;
    }

    return boRet;
}

INT ofxIds::InitCamera (HIDS *hCam, HWND hWnd)
{
    INT nRet = is_InitCamera (hCam, hWnd);
    /************************************************************************************************/
    /*                                                                                              */
    /*  If the camera returns with "IS_STARTER_FW_UPLOAD_NEEDED", an upload of a new firmware       */
    /*  is necessary. This upload can take several seconds. We recommend to check the required      */
    /*  time with the function is_GetDuration().                                                    */
    /*                                                                                              */
    /*  In this case, the camera can only be opened if the flag "IS_ALLOW_STARTER_FW_UPLOAD"        */
    /*  is "OR"-ed to m_hCam. This flag allows an automatic upload of the firmware.                 */
    /*                                                                                              */
    /************************************************************************************************/
    if (nRet == IS_STARTER_FW_UPLOAD_NEEDED)
    {
        // Time for the firmware upload = 25 seconds by default
        INT nUploadTime = 25000;
        is_GetDuration (NULL, IS_SE_STARTER_FW_UPLOAD, &nUploadTime);

        char Str1[] = { "This camera requires a new firmware. The upload will take about" };
        char Str2[] = { "seconds. Please wait ..." };
        char Str3[100];
        sprintf (Str3, "%s %d %s", Str1, nUploadTime / 1000, Str2);
        MessageBox (NULL, Str3, "", MB_ICONWARNING);

        // Try again to open the camera. This time we allow the automatic upload of the firmware by
        // specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
        *hCam = (HIDS) (((INT)*hCam) | IS_ALLOW_STARTER_FW_UPLOAD);
        nRet = is_InitCamera (hCam, hWnd);
    }

    return nRet;
}


void ofxIds::GetMaxImageSize(INT *pnSizeX, INT *pnSizeY)
{
    /*
    // Check if the camera supports an arbitrary AOI
    INT nAOISupported = 0;
    BOOL bAOISupported = TRUE;
    if (is_ImageFormat(m_hCam,
                       IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,
                       (void*)&nAOISupported,
                       sizeof(nAOISupported)) == IS_SUCCESS)
    {
        bAOISupported = (nAOISupported != 0);
    }

    if (bAOISupported)
    {
    */
        // Get maximum image size
        SENSORINFO sInfo;
        is_GetSensorInfo (m_hCam, &sInfo);
        *pnSizeX = sInfo.nMaxWidth;
        *pnSizeY = sInfo.nMaxHeight;
        /*
    }
    else
    {
        // Get image size of the current format
        *pnSizeX = is_SetImageSize(m_hCam, IS_GET_IMAGE_SIZE_X, 0);
        *pnSizeY = is_SetImageSize(m_hCam, IS_GET_IMAGE_SIZE_Y, 0);
    }
    */
}

int ofxIds::InitDisplayMode()
{
    int nRet = IS_NO_SUCCESS;

    if (m_hCam == NULL)
    {
        return IS_NO_SUCCESS;
    }

    // if some image memory exist already then free it
	if (m_pcImageMemory != NULL)
    {
  		nRet = is_FreeImageMem (m_hCam, m_pcImageMemory, m_nMemoryId);
    }

    m_pcImageMemory = NULL;

    switch(m_nDispModeSel)
    {
        case e_disp_mode_directdraw:
            // if initializiing the direct draw mode succeeded then
            // leave the switch tree else run through to bitmap mode
            nRet = is_SetDisplayMode( m_hCam,
                    IS_SET_DM_DIRECTDRAW |
                    IS_SET_DM_ALLOW_OVERLAY |
                    IS_SET_DM_ALLOW_SCALING );
            if(nRet == IS_SUCCESS )
            {
                // setup the color depth to the current VGA setting
                nRet = is_GetColorDepth( m_hCam, &m_nBitsPerPixel, &m_nColorMode );
                break;
            }
            else
                m_nDispModeSel = e_disp_mode_bitmap;

        case e_disp_mode_bitmap:
            nRet = is_SetDisplayMode( m_hCam, IS_SET_DM_DIB);
            if(nRet == IS_SUCCESS)
            {
                if( m_si.nColorMode == IS_COLORMODE_BAYER )
                {
                    // for color camera models use RGB24 mode
                    m_nColorMode = IS_SET_CM_RGB24;
                    m_nBitsPerPixel = 24;
                    ofLog(OF_LOG_VERBOSE, "using IS_COLORMODE_BAYER at 24 bpp");
                }
                else if( m_si.nColorMode == IS_COLORMODE_CBYCRY )
                {
                    // for CBYCRY camera models use RGB32 mode
                    m_nColorMode = IS_SET_CM_RGB32;
                    m_nBitsPerPixel = 32;
                    ofLog(OF_LOG_VERBOSE, "using IS_COLORMODE_CBYCRY at 32 bpp");
                }
                else
                {
                    // for monochrome camera models use Y8 mode
                    m_nColorMode = IS_SET_CM_Y8;
                    m_nBitsPerPixel = 8;
                    ofLog(OF_LOG_VERBOSE, "using monochrome mode at 8 bpp");
                }

                // allocate an image memory.
                nRet = is_AllocImageMem(m_hCam,
                                        m_nSizeX,
                                        m_nSizeY,
                                        m_nBitsPerPixel,
                                        &m_pcImageMemory,
                                        &m_nMemoryId );
            }   // end if(nRet == IS_SUCCESS)
            break;
    }

    if(nRet == IS_SUCCESS)
    {
        // set the image memory only for the bitmap mode when allocated
        if(m_pcImageMemory != NULL)
            nRet = is_SetImageMem( m_hCam, m_pcImageMemory, m_nMemoryId );

        // set the desired color mode
        nRet = is_SetColorMode( m_hCam, m_nColorMode );
        // set the image size to capture
        nRet = is_SetImageSize( m_hCam, m_nSizeX, m_nSizeY );
    }   // end if(nRet == IS_SUCCESS)

    return nRet;
}
