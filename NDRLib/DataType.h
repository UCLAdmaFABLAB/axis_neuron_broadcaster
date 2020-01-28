#pragma once

#include <stdint.h>

/**********************************************************
 *               Customer data types definition           *
 **********************************************************/
#ifdef __OS_XUN__            // Mac OS X, Linux or Unix like OS data type definition
#define __stdcall            // Empty
#endif

#ifndef NBOOL
#define NBOOL int
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_PACKETSIZE				100000

 // Reference to a TCP/IP socket. It is only used as an ID to identify what connector is used.
#define SOCKET_REF void*

// Socket status
typedef enum _SocketStatus
{
    CS_Running,              // Socket is working correctly
    CS_Starting,             // Is trying to start service
    CS_OffWork,              // Not working
}SocketStatus;


#pragma pack(push, 1)

// Data version
typedef union DataVersion
{
    uint32_t _VersionMask;
    struct
    {
        uint8_t BuildNumb;      // Build number
        uint8_t Revision;       // Revision number
        uint8_t Minor;          // Subversion number
        uint8_t Major;          // Major version number
    };
}DATA_VER;

// Header format of BVH data
typedef struct _BvhDataHeader
{
    uint16_t   Token1;           // Package start token: 0xDDFF
    DATA_VER   DataVersion;      // Version of community data format. e.g.: 1.0.0.2
    uint16_t   DataCount;        // Values count
    uint8_t    WithDisp;         // With/out displacement
    uint8_t    WithReference;    // With/out reference bone data at first
    uint32_t   AvatarIndex;      // Avatar index
    uint8_t    AvatarName[32];   // Avatar name
    uint32_t   FrameIndex;       // Frame data index
    uint32_t   Reserved;         // Reserved, only enable this package has 64bytes length
    uint32_t   Reserved1;        // Reserved, only enable this package has 64bytes length
    uint32_t   Reserved2;        // Reserved, only enable this package has 64bytes length
    uint16_t   Token2;           // Package end token: 0xEEFF
}BvhDataHeader;

// Header format of BVH data
typedef struct _CalcDataHeader
{
    uint16_t   Token1;          // Package start token: 0x88FF
    DATA_VER   DataVersion;     // Version of community data format. e.g.: 1.0.0.3
    uint32_t   DataCount;       // Values count
    uint32_t   AvatarIndex;     // Avatar index
    uint8_t    AvatarName[32];  // Avatar name
    uint32_t   FrameIndex;      // Frame data index
    uint32_t   Reserved1;       // Reserved, only enable this package has 64bytes length
    uint32_t   Reserved2;       // Reserved, only enable this package has 64bytes length
    uint32_t   Reserved3;       // Reserved, only enable this package has 64bytes length
    uint16_t   Token2;          // Package end token: 0x99FF
}CalcDataHeader;

typedef enum _CommandIdentity
{
    Cmd_ZeroOutPosition = 1001,     // zero out
    Cmd_ZeroOutAllAvatar = 1002,    // zero out all
    Cmd_BvhRotation = 1003,         // bvh rotation
    Cmd_BoneSize = 1004,            // bone size
    Cmd_BvhHeader = 1005,           // bvh header
}CmdId;

// Header format of Cmd data
typedef struct
{
    uint16_t Token1;            // Command start token: 0xAAFF
    uint16_t CommandType;
    uint16_t CommandId;
    uint32_t nDataBytes;        // Num bytes in payload
    uint32_t nReserved;         // To align with a multiple of '8'
    uint16_t Token2;            // Package end token: 0xBBFF
}CommandPackHeader;

typedef struct
{
    CommandPackHeader cmdPackHeader;
    union
    {
        unsigned char  cData[MAX_PACKETSIZE];
        char           szData[MAX_PACKETSIZE];
        long           lData[MAX_PACKETSIZE / 4];
        unsigned long  ulData[MAX_PACKETSIZE / 4];
        float          fData[MAX_PACKETSIZE / 4];
    } Data;                                 // Payload
} CommandPack;

// Bone dimensions, unit: meter
typedef struct _BoneDimension
{
    float Head;              // Bone length of head
    float Neck;              // Bone length of neck
    float Body;              // Length of body
    float ShoulderWidth;     // Width of shoulder
    float UpperArm;          // Bone length of upper arm
    float Forearm;           // Bone length of forearm
    float Palm;              // Bone length of hand
    float HipWidth;          // Width of hip
    float UpperLeg;          // Bone length of upper leg
    float LowerLeg;          // Bone length of lower leg
    float HeelHeight;        // Heel height
    float FootLength;        // Foot length
}BoneDimension;

// BVH rotate orders
typedef enum _RotateOrders
{
    RO_XZY,
    RO_YXZ,
    RO_XYZ,
    RO_YZX,
    RO_ZXY,
    RO_ZYX,
    RO_Unknown,              // Unknown type
}RotateOrders;


#pragma pack(pop)


/* \brief CommandDataReceived CALLBACK
*  \param customedObj
*		User defined object.
*  \param sender
*      Connector reference of TCP/IP client as identity.
*  \param pack
*		\referce to CommandPack.
*/
typedef bool(__stdcall *CommandDataReceived)(void* customedObj, SOCKET_REF sender, CommandPack* pack);
