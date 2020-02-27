
#include "include/Std_Types.h"
#include "include/Com_Asu_Types.h"
#include "include/Com_Types.h"
#include "include/Com_helper.h"
#include "include/Com_Cfg.h"
#include "include/Com_Buffer.h"


extern const ComConfig_type * ComConfig;

/* Com_Asu_Config declaration*/
extern Com_Asu_Config_type ComAsuConfiguration;
static Com_Asu_Config_type * Com_Asu_Config = &ComAsuConfiguration;


void Com_PackSignalsToPdu(uint16 ComIPuId)
{
	uint8 signalID = 0;
	const ComIPdu_type *IPdu = GET_IPdu(ComIPuId);
	for ( signalID = 0; (IPdu->ComIPduSignalRef[signalID] != NULL); signalID++)
	{

		Com_WriteSignalDataToPduBuffer(IPdu->ComIPduSignalRef[signalID]->ComHandleId, IPdu->ComIPduSignalRef[signalID]->ComSignalDataPtr);
	}
}


void Com_UnPackSignalsFromPdu(uint16 ComIPuId)
{
	uint8 signalID = 0;
//	const ComSignal_type * signal = NULL;
//	const Com_Asu_Signal_type * Asu_Signal = NULL;
	const ComIPdu_type *IPdu = GET_IPdu(ComIPuId);

	for ( signalID = 0; (IPdu->ComIPduSignalRef[signalID] != NULL); signalID++)
	{
//		signal = IPdu->ComIPduSignalRef[signalID];
//		Asu_Signal = GET_AsuSignal(signal->ComHandleId);
//
//		if(Asu_Signal->ComSignalUpdated)
//		{
			Com_ReadSignalDataFromPduBuffer(IPdu->ComIPduSignalRef[signalID]->ComHandleId,IPdu->ComIPduSignalRef[signalID]->ComSignalDataPtr);
//		}
	}
}


void Com_WriteSignalDataToPduBuffer(const uint16 signalId, const void *signalData)
{
    /*awel bit llsignal fe pdu buffer */
    uint32 bitPosition;
    uint8 *pduBufferBytes = NULL;
    uint8 *dataBytes = NULL;
    const ComSignal_type * Signal =  GET_Signal(signalId);
    // Get PDU
    const ComIPdu_type *IPdu = GET_IPdu(Signal->ComIPduHandleId);

    void * const pduBuffer = IPdu->ComIPduDataPtr;

    bitPosition = Signal->ComBitPosition;

    pduBufferBytes = (uint8 *)pduBuffer;

    dataBytes = (uint8 *) signalData;

    if(NORMAL==IPdu->ComIPduType)
    {
    uint64 signal_mask = power(2,Signal->ComBitSize)-1;
    signal_mask <<= bitPosition;
    signal_mask  = ~ signal_mask;

    uint64 signal_data_shifted = (uint64)dataBytes;
    signal_data_shifted <<= bitPosition ;
    signal_mask = signal_mask | signal_data_shifted ;
    *pduBufferBytes= (*pduBufferBytes) & signal_mask ;
    }
}



/*
void Com_WriteSignalDataToPduBuffer(const uint16 signalId, const void *signalData)
{
	uint32 bitPosition;
	uint8 data;
	uint8 mask;
	uint8 pduMask;
	uint8 signalMask;
	uint8 *pduBufferBytes = NULL;
	uint8 *pduBeforChange = NULL;
	uint8 *dataBytes = NULL;
	uint8 signalLength;
	uint8 BitOffsetInByte;
	uint8 pduStartByte;
	uint8 i;
	Com_Asu_IPdu_type *Asu_IPdu = NULL;



	const ComSignal_type * Signal =  GET_Signal(signalId);
	// Get PDU
	const ComIPdu_type *IPdu = GET_IPdu(Signal->ComIPduHandleId);
	void * const pduBuffer = IPdu->ComIPduDataPtr;

	bitPosition = Signal->ComBitPosition;
	BitOffsetInByte = bitPosition%8;
	pduStartByte = bitPosition / 8;
	pduBufferBytes = (uint8 *)pduBuffer;
	dataBytes = (uint8 *) signalData;
	signalLength = Signal->ComBitSize/8;
	pduBeforChange = pduBufferBytes;

	pduBufferBytes += pduStartByte;
	uint8 x;
	for(i = 0; i<=signalLength; i++)
	{
	    pduMask = 255;
	    signalMask = 255;
        if( i == 0)
        {
            pduMask = pduMask >> (8 - BitOffsetInByte);
            signalMask = signalMask >> BitOffsetInByte;
            *pduBufferBytes = (* pduBufferBytes) & pduMask;
            data = (* dataBytes) & signalMask;
            data = data << BitOffsetInByte;
            *pduBufferBytes = (* pduBufferBytes) | data;
            x= *pduBufferBytes;
            pduBufferBytes ++;
        }
        else if(i==signalLength)
        {
            pduMask = pduMask << BitOffsetInByte;
            signalMask = signalMask << (8 - BitOffsetInByte);
            *pduBufferBytes = (* pduBufferBytes) & pduMask;
            data = (* dataBytes) & signalMask;
            data = data >> (8 - BitOffsetInByte);
            *pduBufferBytes = (* pduBufferBytes) | data;
            x= *pduBufferBytes;
        }
        else
        {
            pduMask = pduMask << BitOffsetInByte;
            signalMask = signalMask << (8 - BitOffsetInByte);
            *pduBufferBytes = (* pduBufferBytes) & pduMask;
            data = (* dataBytes) & signalMask;
            data = data >> (8 - BitOffsetInByte);
            *pduBufferBytes = (* pduBufferBytes) | data;

            dataBytes++;

            pduMask = 255;
            signalMask = 255;
            pduMask = pduMask >> (8 - BitOffsetInByte);
            signalMask = signalMask >> BitOffsetInByte;
            *pduBufferBytes = (* pduBufferBytes) & pduMask;
            data = (* dataBytes) & signalMask;
            data = data << BitOffsetInByte;
            *pduBufferBytes = (* pduBufferBytes) | data;
            x= *pduBufferBytes;
            pduBufferBytes ++;

        }
	}
}
*/


void Com_ReadSignalDataFromPduBuffer(const uint16 signalId, void *signalData)
{
	uint8 signalLength;
	uint8 data;
	uint8 BitOffsetInByte;
	uint32 bitPosition;
	uint8 pduStartByte;
	uint8 pduMask;
	uint8 * dataBytes = NULL;
	uint8 i;
	const uint8 *pduBufferBytes = NULL;
	uint8 * signalDataBytes = NULL;

	const ComSignal_type * Signal = GET_Signal(signalId);
	const ComIPdu_type *IPdu = GET_IPdu(Signal->ComIPduHandleId);
	const void * pduBuffer = IPdu->ComIPduDataPtr;

	bitPosition = Signal->ComBitPosition;
	signalLength = Signal->ComBitSize / 8;
	BitOffsetInByte = bitPosition % 8;
	pduStartByte = bitPosition / 8;

	dataBytes = (uint8 *) signalData;
	memset(signalData, 0, signalLength);
	pduBufferBytes = (const uint8 *)pduBuffer;
	pduBufferBytes += pduStartByte;


	uint8 x;
	for(i = 0; i<=signalLength; i++)
    {
        pduMask = 255;
        if( i == 0)
        {
            pduMask = pduMask << BitOffsetInByte;
            data = (* pduBufferBytes) & pduMask;
            data = data >> BitOffsetInByte;
            *dataBytes = *dataBytes | data;
            x= *dataBytes;
            pduBufferBytes ++;
        }
        else if(i==signalLength)
        {
            pduMask = pduMask >> (8-BitOffsetInByte);
            data = (* pduBufferBytes) & pduMask;
            data = data << (8-BitOffsetInByte);
            *dataBytes = (* dataBytes) | data;
            x= *dataBytes;
        }
        else
        {
            pduMask = pduMask >> (8-BitOffsetInByte);
            data = (* pduBufferBytes) & pduMask;
            data = data << (8-BitOffsetInByte);
            *dataBytes = (* dataBytes) | data;

            dataBytes++;

            pduMask = 255;
            pduMask = pduMask << BitOffsetInByte;
            data = (* pduBufferBytes) & pduMask;
            data = data >> BitOffsetInByte;
            *dataBytes = (* pduBufferBytes) | data;
            x= *dataBytes;
            pduBufferBytes ++;

        }
    }
}



void Com_WriteSignalDataToSignalBuffer (const uint16 signalId, const void * signalData)
{
	const ComSignal_type * Signal =  GET_Signal(signalId);
	memcpy(Signal->ComSignalDataPtr, signalData, Asu_Ceil(Signal->ComBitSize));
	uint32_t x= *((uint32*)(Signal->ComSignalDataPtr));
}


void Com_ReadSignalDataFromSignalBuffer (const uint16 signalId,  void * signalData)
{
	const ComSignal_type * Signal =  GET_Signal(signalId);
	memcpy(signalData, Signal->ComSignalDataPtr, Signal->ComBitSize/8);
}



//void inline unlockBuffer(PduIdType id)
//{
//	Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(id);
//    Asu_IPdu->PduBufferState.Locked=FALSE;
//    Asu_IPdu->PduBufferState.CurrentPosition=0;
//}

//void inline lockBuffer(PduIdType id)
//{
//	Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(id);
//	Asu_IPdu->PduBufferState.Locked=TRUE;
//}



