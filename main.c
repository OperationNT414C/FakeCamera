#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/camera.h>
#include <taihen.h>

#define NB_CAM 2

static SceUID g_hooks[39];


// Open - Close

static int cameraOpened[NB_CAM] = {0, 0};
static uint16_t framerate[NB_CAM];

static tai_hook_ref_t ref_hook0;
static int hook_sceCameraOpen(int devnum, SceCameraInfo *pInfo)
{
    int res = TAI_CONTINUE(int, ref_hook0, devnum, pInfo);
    
    if ((unsigned int)devnum < NB_CAM && NULL != pInfo)
    {
        cameraOpened[devnum] = 1;
        framerate[devnum] = pInfo->framerate;

        if (res < 0)
        {
            pInfo->width = (640 >> (pInfo->resolution-1));
            pInfo->height = (480 >> (pInfo->resolution-1));
            res = 0;
        }
    }

    return res;
}

static tai_hook_ref_t ref_hook1;
static int hook_sceCameraClose(int devnum)
{
    int res = TAI_CONTINUE(int, ref_hook1, devnum);
    
    if ((unsigned int)devnum < NB_CAM)
    {
        cameraOpened[devnum] = 0;
        if (res < 0) res = 0;
    }
    
    return res;
}

// Start - Stop

static int cameraActive[NB_CAM] = {0, 0};
static uint64_t initTimeStamp[NB_CAM];
static uint64_t prevTimeStamp[NB_CAM];

static tai_hook_ref_t ref_hook2;
static int hook_sceCameraStart(int devnum)
{
    int res = TAI_CONTINUE(int, ref_hook2, devnum);
    
    if ((unsigned int)devnum < NB_CAM && cameraOpened[devnum])
    {
        cameraActive[devnum] = 1;
        initTimeStamp[devnum] = sceKernelGetProcessTimeWide();
        prevTimeStamp[devnum] = initTimeStamp[devnum];
        if (res < 0) res = 0;
    }
    
    return res;
}

static tai_hook_ref_t ref_hook3;
static int hook_sceCameraStop(int devnum)
{
    int res = TAI_CONTINUE(int, ref_hook3, devnum);
    
    if ((unsigned int)devnum < NB_CAM)
    {
        cameraActive[devnum] = 0;
        if (res < 0) res = 0;
    }
    
    return res;
}

// Read

static tai_hook_ref_t ref_hook4;
static int hook_sceCameraRead(int devnum, SceCameraRead *pRead)
{
    int res = TAI_CONTINUE(int, ref_hook4, devnum, pRead);
    
    if ((unsigned int)devnum < NB_CAM && NULL != pRead && cameraActive[devnum])
    {
        uint64_t newTimeStamp = sceKernelGetProcessTimeWide();
        uint64_t fakeTimeStamp = (newTimeStamp+prevTimeStamp[devnum])>>1;
        uint64_t fakeFrame = (((fakeTimeStamp-initTimeStamp[devnum])*framerate[devnum])>>22) + 1;
        if (res < 0)
        {
            pRead->frame = fakeFrame;
            pRead->timestamp = fakeTimeStamp;
            res = 0;
        }
        prevTimeStamp[devnum] = newTimeStamp;
    }

    return res;
}

// Active

static tai_hook_ref_t ref_hook5;
static int hook_sceCameraIsActive(int devnum)
{
    int res = TAI_CONTINUE(int, ref_hook5, devnum);
    if ((unsigned int)devnum < NB_CAM && res <= 0) res = cameraActive[devnum];
    return res;
}

// Location

static tai_hook_ref_t ref_hook6;
static int hook_sceCameraGetDeviceLocation(int devnum, SceFVector3 *pLocation)
{
    int res = TAI_CONTINUE(int, ref_hook6, devnum, pLocation);
    if ((unsigned int)devnum < NB_CAM && NULL != pLocation && res < 0) res = 0;
    return res;
}

// Saturation

static int saturation[NB_CAM] = {SCE_CAMERA_SATURATION_0, SCE_CAMERA_SATURATION_0};

static tai_hook_ref_t ref_hook7;
static int hook_sceCameraGetSaturation(int devnum, int *pLevel)
{
    int res = TAI_CONTINUE(int, ref_hook7, devnum, pLevel);
    if ((unsigned int)devnum < NB_CAM && NULL != pLevel && res < 0)
    {
        *pLevel = saturation[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook8;
static int hook_sceCameraSetSaturation(int devnum, int level)
{
    int res = TAI_CONTINUE(int, ref_hook8, devnum, level);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        saturation[devnum] = level;
        res = 0;
    }
    return res;
}

// Brightness

static int brightness[NB_CAM] = {127, 127};

static tai_hook_ref_t ref_hook9;
static int hook_sceCameraGetBrightness(int devnum, int *pLevel)
{
    int res = TAI_CONTINUE(int, ref_hook9, devnum, pLevel);
    if ((unsigned int)devnum < NB_CAM && NULL != pLevel && res < 0)
    {
        *pLevel = brightness[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook10;
static int hook_sceCameraSetBrightness(int devnum, int level)
{
    int res = TAI_CONTINUE(int, ref_hook10, devnum, level);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        brightness[devnum] = level;
        res = 0;
    }
    return res;
}

// Contrast

static int contrast[NB_CAM] = {127, 127};

static tai_hook_ref_t ref_hook11;
static int hook_sceCameraGetContrast(int devnum, int *pLevel)
{
    int res = TAI_CONTINUE(int, ref_hook11, devnum, pLevel);
    if ((unsigned int)devnum < NB_CAM && NULL != pLevel && res < 0)
    {
        *pLevel = contrast[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook12;
static int hook_sceCameraSetContrast(int devnum, int level)
{
    int res = TAI_CONTINUE(int, ref_hook12, devnum, level);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        contrast[devnum] = level;
        res = 0;
    }
    return res;
}

// Sharpness

static int sharpness[NB_CAM] = {SCE_CAMERA_SHARPNESS_100, SCE_CAMERA_SHARPNESS_100};

static tai_hook_ref_t ref_hook13;
static int hook_sceCameraGetSharpness(int devnum, int *pLevel)
{
    int res = TAI_CONTINUE(int, ref_hook13, devnum, pLevel);
    if ((unsigned int)devnum < NB_CAM && NULL != pLevel && res < 0)
    {
        *pLevel = sharpness[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook14;
static int hook_sceCameraSetSharpness(int devnum, int level)
{
    int res = TAI_CONTINUE(int, ref_hook14, devnum, level);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        sharpness[devnum] = level;
        res = 0;
    }
    return res;
}

// Reverse

static int reverse[NB_CAM] = {SCE_CAMERA_REVERSE_OFF, SCE_CAMERA_REVERSE_OFF};

static tai_hook_ref_t ref_hook15;
static int hook_sceCameraGetReverse(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook15, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = reverse[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook16;
static int hook_sceCameraSetReverse(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook16, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        reverse[devnum] = mode;
        res = 0;
    }
    return res;
}

// Effect

static int effect[NB_CAM] = {SCE_CAMERA_EFFECT_NORMAL, SCE_CAMERA_EFFECT_NORMAL};

static tai_hook_ref_t ref_hook17;
static int hook_sceCameraGetEffect(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook17, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = effect[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook18;
static int hook_sceCameraSetEffect(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook18, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        effect[devnum] = mode;
        res = 0;
    }
    return res;
}

// EV

static int ev[NB_CAM] = {SCE_CAMERA_EV_POSITIVE_0, SCE_CAMERA_EV_POSITIVE_0};

static tai_hook_ref_t ref_hook19;
static int hook_sceCameraGetEV(int devnum, int *pLevel)
{
    int res = TAI_CONTINUE(int, ref_hook19, devnum, pLevel);
    if ((unsigned int)devnum < NB_CAM && NULL != pLevel && res < 0)
    {
        *pLevel = ev[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook20;
static int hook_sceCameraSetEV(int devnum, int level)
{
    int res = TAI_CONTINUE(int, ref_hook20, devnum, level);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        ev[devnum] = level;
        res = 0;
    }
    return res;
}

// Zoom

static int zoom[NB_CAM] = {10, 10};

static tai_hook_ref_t ref_hook21;
static int hook_sceCameraGetZoom(int devnum, int *pLevel)
{
    int res = TAI_CONTINUE(int, ref_hook21, devnum, pLevel);
    if ((unsigned int)devnum < NB_CAM && NULL != pLevel && res < 0)
    {
        *pLevel = zoom[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook22;
static int hook_sceCameraSetZoom(int devnum, int level)
{
    int res = TAI_CONTINUE(int, ref_hook22, devnum, level);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        zoom[devnum] = level;
        res = 0;
    }
    return res;
}

// AntiFlicker

static int antiFlicker[NB_CAM] = {SCE_CAMERA_ANTIFLICKER_AUTO, SCE_CAMERA_ANTIFLICKER_AUTO};

static tai_hook_ref_t ref_hook23;
static int hook_sceCameraGetAntiFlicker(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook23, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = antiFlicker[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook24;
static int hook_sceCameraSetAntiFlicker(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook24, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        antiFlicker[devnum] = mode;
        res = 0;
    }
    return res;
}

// ISO

static int iso[NB_CAM] = {SCE_CAMERA_ISO_AUTO, SCE_CAMERA_ISO_AUTO};

static tai_hook_ref_t ref_hook25;
static int hook_sceCameraGetISO(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook25, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = iso[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook26;
static int hook_sceCameraSetISO(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook26, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        iso[devnum] = mode;
        res = 0;
    }
    return res;
}

// Gain

static int gain[NB_CAM] = {SCE_CAMERA_GAIN_AUTO, SCE_CAMERA_GAIN_AUTO};

static tai_hook_ref_t ref_hook27;
static int hook_sceCameraGetGain(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook27, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = gain[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook28;
static int hook_sceCameraSetGain(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook28, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        gain[devnum] = mode;
        res = 0;
    }
    return res;
}

// WhiteBalance

static int whiteBalance[NB_CAM] = {SCE_CAMERA_WB_AUTO, SCE_CAMERA_WB_AUTO};

static tai_hook_ref_t ref_hook29;
static int hook_sceCameraGetWhiteBalance(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook29, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = whiteBalance[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook30;
static int hook_sceCameraSetWhiteBalance(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook30, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        whiteBalance[devnum] = mode;
        res = 0;
    }
    return res;
}

// Backlight

static int backlight[NB_CAM] = {SCE_CAMERA_BACKLIGHT_OFF, SCE_CAMERA_BACKLIGHT_OFF};

static tai_hook_ref_t ref_hook31;
static int hook_sceCameraGetBacklight(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook31, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = backlight[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook32;
static int hook_sceCameraSetBacklight(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook32, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        backlight[devnum] = mode;
        res = 0;
    }
    return res;
}

// Nightmode

static int nightmode[NB_CAM] = {SCE_CAMERA_NIGHTMODE_OFF, SCE_CAMERA_NIGHTMODE_OFF};

static tai_hook_ref_t ref_hook33;
static int hook_sceCameraGetNightmode(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook33, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = nightmode[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook34;
static int hook_sceCameraSetNightmode(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook34, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        nightmode[devnum] = mode;
        res = 0;
    }
    return res;
}

// ExposureCeiling

static int exposureCeiling[NB_CAM] = {0, 0};

static tai_hook_ref_t ref_hook35;
static int hook_sceCameraGetExposureCeiling(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook35, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = exposureCeiling[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook36;
static int hook_sceCameraSetExposureCeiling(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook36, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        exposureCeiling[devnum] = mode;
        res = 0;
    }
    return res;
}

// AutoControlHold

static int autoControlHold[NB_CAM] = {0, 0};

static tai_hook_ref_t ref_hook37;
static int hook_sceCameraGetAutoControlHold(int devnum, int *pMode)
{
    int res = TAI_CONTINUE(int, ref_hook37, devnum, pMode);
    if ((unsigned int)devnum < NB_CAM && NULL != pMode && res < 0)
    {
        *pMode = autoControlHold[devnum];
        res = 0;
    }
    return res;
}

static tai_hook_ref_t ref_hook38;
static int hook_sceCameraSetAutoControlHold(int devnum, int mode)
{
    int res = TAI_CONTINUE(int, ref_hook38, devnum, mode);
    if ((unsigned int)devnum < NB_CAM && res < 0)
    {
        autoControlHold[devnum] = mode;
        res = 0;
    }
    return res;
}


void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
    g_hooks[0] = taiHookFunctionImport(&ref_hook0, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xA462F801, // sceCameraOpen
                                        hook_sceCameraOpen);
    g_hooks[1] = taiHookFunctionImport(&ref_hook1, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xCD6E1CFC, // sceCameraClose
                                        hook_sceCameraClose);
    g_hooks[2] = taiHookFunctionImport(&ref_hook2, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xA8FEAE35, // sceCameraStart
                                        hook_sceCameraStart);
    g_hooks[3] = taiHookFunctionImport(&ref_hook3, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x1DD9C9CE, // sceCameraStop
                                        hook_sceCameraStop);
    g_hooks[4] = taiHookFunctionImport(&ref_hook4, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x79B5C2DE, // sceCameraRead
                                        hook_sceCameraRead);
    g_hooks[5] = taiHookFunctionImport(&ref_hook5, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x103A75B8, // sceCameraIsActive
                                        hook_sceCameraIsActive);
    g_hooks[6] = taiHookFunctionImport(&ref_hook6, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x274EF751, // sceCameraGetDeviceLocation
                                        hook_sceCameraGetDeviceLocation);

    // Getters - Setters
    g_hooks[7] = taiHookFunctionImport(&ref_hook7, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x624F7653, // sceCameraGetSaturation
                                        hook_sceCameraGetSaturation);
    g_hooks[8] = taiHookFunctionImport(&ref_hook8, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xF9F7CA3D, // sceCameraSetSaturation
                                        hook_sceCameraSetSaturation);
    g_hooks[9] = taiHookFunctionImport(&ref_hook9, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x85D5951D, // sceCameraGetBrightness
                                        hook_sceCameraGetBrightness);
    g_hooks[10] = taiHookFunctionImport(&ref_hook10, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x98D71588, // sceCameraSetBrightness
                                        hook_sceCameraSetBrightness);
    g_hooks[11] = taiHookFunctionImport(&ref_hook11, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x8FBE84BE, // sceCameraGetContrast
                                        hook_sceCameraGetContrast);
    g_hooks[12] = taiHookFunctionImport(&ref_hook12, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x6FB2900, // sceCameraSetContrast
                                        hook_sceCameraSetContrast);
    g_hooks[13] = taiHookFunctionImport(&ref_hook13, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xAA72C3DC, // sceCameraGetSharpness
                                        hook_sceCameraGetSharpness);
    g_hooks[14] = taiHookFunctionImport(&ref_hook14, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xD1A5BB0B, // sceCameraSetSharpness
                                        hook_sceCameraSetSharpness);
    g_hooks[15] = taiHookFunctionImport(&ref_hook15, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x44F6043F, // sceCameraGetReverse
                                        hook_sceCameraGetReverse);
    g_hooks[16] = taiHookFunctionImport(&ref_hook16, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x1175F477, // sceCameraSetReverse
                                        hook_sceCameraSetReverse);
    g_hooks[17] = taiHookFunctionImport(&ref_hook17, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x7E8EF3B2, // sceCameraGetEffect
                                        hook_sceCameraGetEffect);
    g_hooks[18] = taiHookFunctionImport(&ref_hook18, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xE9D2CFB1, // sceCameraSetEffect
                                        hook_sceCameraSetEffect);
    g_hooks[19] = taiHookFunctionImport(&ref_hook19, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x8B5E6147, // sceCameraGetEV
                                        hook_sceCameraGetEV);
    g_hooks[20] = taiHookFunctionImport(&ref_hook20, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x62AFF0B8, // sceCameraSetEV
                                        hook_sceCameraSetEV);
    g_hooks[21] = taiHookFunctionImport(&ref_hook21, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x06D3816C, // sceCameraGetZoom
                                        hook_sceCameraGetZoom);
    g_hooks[22] = taiHookFunctionImport(&ref_hook22, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xF7464216, // sceCameraSetZoom
                                        hook_sceCameraSetZoom);
    g_hooks[23] = taiHookFunctionImport(&ref_hook23, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x9FDACB99, // sceCameraGetAntiFlicker
                                        hook_sceCameraGetAntiFlicker);
    g_hooks[24] = taiHookFunctionImport(&ref_hook24, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xE312958A, // sceCameraSetAntiFlicker
                                        hook_sceCameraSetAntiFlicker);
    g_hooks[25] = taiHookFunctionImport(&ref_hook25, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x4EBD5C68, // sceCameraGetISO
                                        hook_sceCameraGetISO);
    g_hooks[26] = taiHookFunctionImport(&ref_hook26, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x3CF630A1, // sceCameraSetISO
                                        hook_sceCameraSetISO);
    g_hooks[27] = taiHookFunctionImport(&ref_hook27, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x2C36D6F3, // sceCameraGetGain
                                        hook_sceCameraGetGain);
    g_hooks[28] = taiHookFunctionImport(&ref_hook28, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xE65CFE86, // sceCameraSetGain
                                        hook_sceCameraSetGain);
    g_hooks[29] = taiHookFunctionImport(&ref_hook29, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xDBFFA1DA, // sceCameraGetWhiteBalance
                                        hook_sceCameraGetWhiteBalance);                                        
    g_hooks[30] = taiHookFunctionImport(&ref_hook30, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x4D4514AC, // sceCameraSetWhiteBalance
                                        hook_sceCameraSetWhiteBalance);
    g_hooks[31] = taiHookFunctionImport(&ref_hook31, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x8DD1292B, // sceCameraGetBacklight
                                        hook_sceCameraGetBacklight);
    g_hooks[32] = taiHookFunctionImport(&ref_hook32, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0xAE071044, // sceCameraSetBacklight
                                        hook_sceCameraSetBacklight);
    g_hooks[33] = taiHookFunctionImport(&ref_hook33, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x12B6FF26, // sceCameraGetNightmode
                                        hook_sceCameraGetNightmode);
    g_hooks[34] = taiHookFunctionImport(&ref_hook34, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x3F26233E, // sceCameraSetNightmode
                                        hook_sceCameraSetNightmode);
    g_hooks[35] = taiHookFunctionImport(&ref_hook35, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x5FA5B1BB, // sceCameraGetExposureCeiling
                                        hook_sceCameraGetExposureCeiling);
    g_hooks[36] = taiHookFunctionImport(&ref_hook36, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x4F34BEE, // sceCameraSetExposureCeiling
                                        hook_sceCameraSetExposureCeiling);
    g_hooks[37] = taiHookFunctionImport(&ref_hook37, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x06A21BBB, // sceCameraGetAutoControlHold
                                        hook_sceCameraGetAutoControlHold);
    g_hooks[38] = taiHookFunctionImport(&ref_hook38, 
                                        TAI_MAIN_MODULE,
                                        0xDA91B3ED, // SceCamera
                                        0x3A0DABBD, // sceCameraSetAutoControlHold
                                        hook_sceCameraSetAutoControlHold);

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
    if (g_hooks[0] >= 0) taiHookRelease(g_hooks[0], ref_hook0);
    if (g_hooks[1] >= 0) taiHookRelease(g_hooks[1], ref_hook1);
    if (g_hooks[2] >= 0) taiHookRelease(g_hooks[2], ref_hook2);
    if (g_hooks[3] >= 0) taiHookRelease(g_hooks[3], ref_hook3);
    if (g_hooks[4] >= 0) taiHookRelease(g_hooks[4], ref_hook4);
    if (g_hooks[5] >= 0) taiHookRelease(g_hooks[5], ref_hook5);
    if (g_hooks[6] >= 0) taiHookRelease(g_hooks[6], ref_hook6);
    if (g_hooks[7] >= 0) taiHookRelease(g_hooks[7], ref_hook7);
    if (g_hooks[8] >= 0) taiHookRelease(g_hooks[8], ref_hook8);
    if (g_hooks[9] >= 0) taiHookRelease(g_hooks[9], ref_hook9);
    if (g_hooks[10] >= 0) taiHookRelease(g_hooks[10], ref_hook10);
    if (g_hooks[11] >= 0) taiHookRelease(g_hooks[11], ref_hook11);
    if (g_hooks[12] >= 0) taiHookRelease(g_hooks[12], ref_hook12);
    if (g_hooks[13] >= 0) taiHookRelease(g_hooks[13], ref_hook13);
    if (g_hooks[14] >= 0) taiHookRelease(g_hooks[14], ref_hook14);
    if (g_hooks[15] >= 0) taiHookRelease(g_hooks[15], ref_hook15);
    if (g_hooks[16] >= 0) taiHookRelease(g_hooks[16], ref_hook16);
    if (g_hooks[17] >= 0) taiHookRelease(g_hooks[17], ref_hook17);
    if (g_hooks[18] >= 0) taiHookRelease(g_hooks[18], ref_hook18);
    if (g_hooks[19] >= 0) taiHookRelease(g_hooks[19], ref_hook19);
    if (g_hooks[20] >= 0) taiHookRelease(g_hooks[20], ref_hook20);
    if (g_hooks[21] >= 0) taiHookRelease(g_hooks[21], ref_hook21);
    if (g_hooks[22] >= 0) taiHookRelease(g_hooks[22], ref_hook22);
    if (g_hooks[23] >= 0) taiHookRelease(g_hooks[23], ref_hook23);
    if (g_hooks[24] >= 0) taiHookRelease(g_hooks[24], ref_hook24);
    if (g_hooks[25] >= 0) taiHookRelease(g_hooks[25], ref_hook25);
    if (g_hooks[26] >= 0) taiHookRelease(g_hooks[26], ref_hook26);
    if (g_hooks[27] >= 0) taiHookRelease(g_hooks[27], ref_hook27);
    if (g_hooks[28] >= 0) taiHookRelease(g_hooks[28], ref_hook28);
    if (g_hooks[29] >= 0) taiHookRelease(g_hooks[29], ref_hook29);
    if (g_hooks[30] >= 0) taiHookRelease(g_hooks[30], ref_hook30);
    if (g_hooks[31] >= 0) taiHookRelease(g_hooks[31], ref_hook31);
    if (g_hooks[32] >= 0) taiHookRelease(g_hooks[32], ref_hook32);
    if (g_hooks[33] >= 0) taiHookRelease(g_hooks[33], ref_hook33);
    if (g_hooks[34] >= 0) taiHookRelease(g_hooks[34], ref_hook34);
    if (g_hooks[35] >= 0) taiHookRelease(g_hooks[35], ref_hook35);
    if (g_hooks[36] >= 0) taiHookRelease(g_hooks[36], ref_hook36);
    if (g_hooks[37] >= 0) taiHookRelease(g_hooks[37], ref_hook37);
    if (g_hooks[38] >= 0) taiHookRelease(g_hooks[38], ref_hook38);
 
    return SCE_KERNEL_STOP_SUCCESS;
}
