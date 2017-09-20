
#ifndef __WUADEC_H__
#define __WUADEC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


//#pragma comment(lib,"libwuad.lib")

/* object types for AAC */
#define MAIN       1
#define LC         2
#define SSR        3
#define LTP        4
#define HE_AAC     5
#define ER_LC     17
#define ER_LTP    19
#define LD        23
#define DRM_ER_LC 27 /* special object type for DRM */

/* header types */
#define RAW        0
#define ADIF       1
#define ADTS       2
#define LATM       3

/* SBR signalling */
#define NO_SBR           0
#define SBR_UPSAMPLED    1
#define SBR_DOWNSAMPLED  2
#define NO_SBR_UPSAMPLED 3

/* library output formats */
#define wuad_FMT_16BIT  1
#define wuad_FMT_24BIT  2
#define wuad_FMT_32BIT  3
#define wuad_FMT_FLOAT  4
#define wuad_FMT_FIXED  wuad_FMT_FLOAT
#define wuad_FMT_DOUBLE 5

/* Capabilities */
#define LC_DEC_CAP           (1<<0) /* Can decode LC */
#define MAIN_DEC_CAP         (1<<1) /* Can decode MAIN */
#define LTP_DEC_CAP          (1<<2) /* Can decode LTP */
#define LD_DEC_CAP           (1<<3) /* Can decode LD */
#define ERROR_RESILIENCE_CAP (1<<4) /* Can decode ER */
#define FIXED_POINT_CAP      (1<<5) /* Fixed point */

/* Channel definitions */
#define FRONT_CHANNEL_CENTER (1)
#define FRONT_CHANNEL_LEFT   (2)
#define FRONT_CHANNEL_RIGHT  (3)
#define SIDE_CHANNEL_LEFT    (4)
#define SIDE_CHANNEL_RIGHT   (5)
#define BACK_CHANNEL_LEFT    (6)
#define BACK_CHANNEL_RIGHT   (7)
#define BACK_CHANNEL_CENTER  (8)
#define LFE_CHANNEL          (9)
#define UNKNOWN_CHANNEL      (0)

/* DRM channel definitions */
#define DRMCH_MONO          1
#define DRMCH_STEREO        2
#define DRMCH_SBR_MONO      3
#define DRMCH_SBR_STEREO    4
#define DRMCH_SBR_PS_STEREO 5


/* A decode call can eat up to wuad_MIN_STREAMSIZE bytes per decoded channel,
   so at least so much bytes per channel should be available in this stream */
#define wuad_MIN_STREAMSIZE 768 /* 6144 bits/channel */


typedef void *wuaDecHandle;

typedef struct wuaDecConfiguration
{
    unsigned char defObjectType;
    unsigned long defSampleRate;
    unsigned char outputFormat;
    unsigned char downMatrix;
    unsigned char useOldADTSFormat;
    unsigned char dontUpSampleImplicitSBR;
} wuaDecConfiguration, *wuaDecConfigurationPtr;

typedef struct wuaDecFrameInfo
{
    unsigned long bytesconsumed;
    unsigned long samples;
    unsigned char channels;
    unsigned char error;
    unsigned long samplerate;

    /* SBR: 0: off, 1: on; upsample, 2: on; downsampled, 3: off; upsampled */
    unsigned char sbr;

    /* MPEG-4 ObjectType */
    unsigned char object_type;

    /* AAC header type; MP4 will be signalled as RAW also */
    unsigned char header_type;

    /* multichannel configuration */
    unsigned char num_front_channels;
    unsigned char num_side_channels;
    unsigned char num_back_channels;
    unsigned char num_lfe_channels;
    unsigned char channel_position[64];

    /* PS: 0: off, 1: on */
    unsigned char ps;
} wuaDecFrameInfo;


_declspec(dllimport) wuaDecHandle __stdcall wuaDecOpen(void);

_declspec(dllimport) wuaDecConfigurationPtr __stdcall wuaDecGetCurrentConfiguration(wuaDecHandle hDecoder);

_declspec(dllimport) unsigned char __stdcall wuaDecSetConfiguration(wuaDecHandle hDecoder,
                                                   wuaDecConfigurationPtr config);

/* Init the library based on info from the AAC file (ADTS/ADIF) */
_declspec(dllimport) long __stdcall wuaDecInit(wuaDecHandle hDecoder,
                              unsigned char *buffer,
                              unsigned long buffer_size,
                              unsigned long *samplerate,
                              unsigned char *channels);

_declspec(dllimport) void __stdcall wuaDecPostSeekReset(wuaDecHandle hDecoder, long frame);

_declspec(dllimport) void __stdcall wuaDecClose(wuaDecHandle hDecoder);

 _declspec(dllimport) void* __stdcall wuaDecDecode(wuaDecHandle hDecoder,
                                 wuaDecFrameInfo *hInfo,
                                 unsigned char *buffer,
                                 unsigned long buffer_size);


#ifdef _WIN32
  #pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
