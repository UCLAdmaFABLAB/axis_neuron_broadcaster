/* \brief Copyright: Copyright 2016 Beijing Noitom Technology Ltd. All Rights reserved.
* Pending Patents: PCT/CN2014/085659 PCT/CN2014/071006
*
* Licensed under the Perception Neuron SDK License Beta Version (the ¡°License");
* You may only use the Perception Neuron SDK when in compliance with the License,
* which is provided at the time of installation or download, or which
* otherwise accompanies this software in the form of either an electronic or a hard copy.
*
* Unless required by applicable law or agreed to in writing, the Perception Neuron SDK
* distributed under the License is provided on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing conditions and
* limitations under the License.
*/

#pragma once

#if WIN32
#include "NeuronDataReader_api.h"
#else
#include <NeuronDataReader_api_mac.h>
#endif
#include "DataType.h"


/**********************************************************
*              SDK version number definition              *
**********************************************************/
#define DR_MAJOR          1
#define DR_MINOR          1
#define DR_REVISION       3
#define DR_BUILDNUMB      435
#define DR_DESC           "DataReader b17"


/**********************************************************
*         Data output callback types definition          *
**********************************************************/
/* FrameDataReceived CALLBACK
*Parameters
*	customedObj
*		User defined object.
*	sender
*      Connector reference of TCP/IP client or UDP service as identity.
*	header
*      A BvhDataHeader type pointer, contains the BVH data information.
*	data
*		Float type array pointer, to output binary data.
*Remarks
*      The related information of the data stream can be obtained from BvhDataHeader.
*/
typedef void(__stdcall *FrameDataReceived)(void* customedObj, SOCKET_REF sender, BvhDataHeader* header, float* data);


/* \brief Calculation frame data received callback
*  \param customedObj
*		User defined object.
*  \param sender
*      Connector reference of TCP/IP client or UDP service as identity.
*  \param header
*      A CalcDataHeader type pointer, contains the calculation data information.
*  \param data
*		Float type array pointer, to output binary data.
*  \remarks
*      The related information of the data stream can be obtained from CalcDataHeader.
*/
typedef void(__stdcall *CalculationDataReceived)(void* customedObj, SOCKET_REF sender, CalcDataHeader* header, float* data);


/* \brief SocketStatusChanged CALLBACK
*  \param customedObj
*		User defined object.
*  \param sender
*      Connector reference of TCP/IP client as identity.
*  \param status
*		Socket status.
*  \param message
*		Socket status description.
*  \remarks
*      As convenient,use BRGetConnectionStatus() to get status manually other than register this callback
*/
typedef void(__stdcall *SocketStatusChanged)(void* customedObj, SOCKET_REF sender, SocketStatus status, char* message);


/* \brief **************************************************
*          Export pure carbon interface/functions        *
*  So that C# or Unity can use it directly               *
**********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

    /************************************************************************/
    /* System API                                                           */
    /************************************************************************/

    /* \brief Register receiving and parsed frame bvh data callback
    *  \param customedObj
    *  \param handle
    */
    NEURONDATAREADER_API void BRRegisterFrameDataCallback(void* customedObj, FrameDataReceived handle);

    /* \brief Register receiving and parsed frame calculation data callback
    *  \param customedObj
    *  \param handle
    */
    NEURONDATAREADER_API void BRRegisterCalculationDataCallback(void* customedObj, CalculationDataReceived handle);

    /* \brief Register socket status callback
    *  \param customedObj
    *  \param handle
    */ 
    NEURONDATAREADER_API void BRRegisterSocketStatusCallback(void* customedObj, SocketStatusChanged handle);

    /************************************************************************/
    /* Socket API                                                           */
    /************************************************************************/

    /* \brief Create a TCP/IP client to connect to server, return a handle of connector, NULL is returned if connect failed.
    *  \param serverIP
    *  \param nPort
    *  \return SOCKET_REF
    */
    NEURONDATAREADER_API SOCKET_REF BRConnectTo(char* serverIP, int nPort);
    
    /* \brief Create a UDP service to receive data at local 'nPort'
    *  \param nPort
    *  \return SOCKET_REF
    */
    NEURONDATAREADER_API SOCKET_REF BRStartUDPServiceAt(int nPort);

    /* \brief Close a socket
    *  \param nPort
    *  \return SOCKET_REF
    */
    NEURONDATAREADER_API void BRCloseSocket(SOCKET_REF sockRef);

    /************************************************************************/
    /* Status API                                                           */
    /************************************************************************/

    /* \brief Check socket status
    *  \param sockRef
    */
    NEURONDATAREADER_API SocketStatus BRGetSocketStatus(SOCKET_REF sockRef);

    /* \brief If any error, you can call 'BRGetLastErrorMessage' to get error information
    *  \param sockRef
    */
    NEURONDATAREADER_API char* BRGetLastErrorMessage();

#ifdef __cplusplus
};
#endif
