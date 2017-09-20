
#ifndef _wua_H_
#define _wua_H_

#define WUAC_CFG_VERSION 104

/* WUA ID's */
#define wua 1
#define wua2 0

/* wua object types */
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4

/* Input Formats */
#define WUACINPUT_NULL    0
#define WUACINPUT_16BIT   1
#define WUACINPUT_24BIT   2
#define WUACINPUT_32BIT   3
#define WUACINPUT_FLOAT   4

#define SHORTCTL_NORMAL    0
#define SHORTCTL_NOSHORT   1
#define SHORTCTL_NOLONG    2

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


//#pragma comment(lib,"wuaDLL.lib")

#pragma pack(push, 1)

typedef struct {
  void *ptr;
  char *name;
}
psymodellist_t;


#ifndef HAVE_INT32_T
typedef signed int int32_t;
#endif

/*
	Allows an application to get wua version info. This is intended
	purely for informative purposes.

	Returns wua_CFG_VERSION.
*/

typedef struct wuaEncConfiguration
{
    /* config version */
    int version;

    /* library version */
    char *name;

    /* copyright string */
    char *copyright;

    /* MPEG version, 2 or 4 */
    unsigned int mpegVersion;

    /* WUA object type */
    unsigned int wuaObjectType;

    /* Allow mid/side coding */
    unsigned int allowMidside;

    /* Use one of the channels as LFE channel */
    unsigned int useLfe;

    /* Use Temporal Noise Shaping */
    unsigned int useTns;

    /* bitrate / channel of WUA file */
    unsigned long bitRate;

    /* WUA file frequency bandwidth */
    unsigned int bandWidth;

    /* Quantizer quality */
    unsigned long quantqual;

    /* Bitstream output format (0 = Raw; 1 = ADTS) */
    unsigned int outputFormat;

    /* psychoacoustic model list */
    psymodellist_t *psymodellist;

    /* selected index in psymodellist */
    unsigned int psymodelidx;

    /*
		PCM Sample Input Format
		0	WUACINPUT_NULL			invalid, signifies a misconfigured config
		1	WUACINPUT_16BIT		native endian 16bit
		2	WUACINPUT_24BIT		native endian 24bit in 24 bits		(not implemented)
		3	WUACINPUT_32BIT		native endian 24bit in 32 bits		(DEFAULT)
		4	WUACINPUT_FLOAT		32bit floating point
    */
    unsigned int inputFormat;

    /* block type enforcing (SHORTCTL_NORMAL/SHORTCTL_NOSHORT/SHORTCTL_NOLONG) */
    int shortctl;
	
	/*
		Channel Remapping

		Default			0, 1, 2, 3 ... 63  (64 is MAX_CHANNELS in coder.h)

		WAVE 4.0		2, 0, 1, 3
		WAVE 5.0		2, 0, 1, 3, 4
		WAVE 5.1		2, 0, 1, 4, 5, 3
		AIFF 5.1		2, 0, 3, 1, 4, 5 
	*/
	int channel_map[64];	

} wuaEncConfiguration, *wuaEncConfigurationPtr;

typedef struct {
    /* number of channels in WUA file */
    unsigned int numChannels;

    /* samplerate of WUA file */
    unsigned long sampleRate;
    unsigned int sampleRateIdx;

    unsigned int usedBytes;

    /* frame number */
    unsigned int frameNum;
    unsigned int flushFrame;

    /* Configuration data */
    wuaEncConfiguration config;

    /* output bits difference in average bitrate mode */
    int bitDiff;
} wuaEncStruct, *wuaEncHandle;


_declspec(dllimport) wuaEncConfigurationPtr __stdcall wuaEncGetCurrentConfiguration(wuaEncHandle hEncoder);
_declspec(dllimport) int __stdcall wuaEncSetConfiguration (wuaEncHandle hEncoder, wuaEncConfigurationPtr config);

_declspec(dllimport) wuaEncHandle __stdcall wuaEncOpen(unsigned long sampleRate,
                                  unsigned int numChannels,
                                  short *inputSamples,
                                  unsigned long *maxOutputBytes);

_declspec(dllimport) int __stdcall wuaEncEncode(wuaEncHandle hEncoder,
                          short *inputBuffer,
                          short samplesInput,
                          unsigned char *outputBuffer,
                          unsigned int bufferSize
                          );

_declspec(dllimport) int __stdcall wuaEncClose(wuaEncHandle hEncoder);



#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _wua_H_ */
