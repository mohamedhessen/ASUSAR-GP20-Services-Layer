
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

/*
void Com_WriteSignalDataToPduBuffer(const uint16 signalId, const void *signalData)
{
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


 */

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

    /*
     *
     * if(ComBitSize_copy==8)
        {
            signalMask = signalMask >> BitOffsetInByte;
        }
        else if (ComBitSize_copy>8)
        {
            ComBitSize_copy = ComBitSize_copy -8 ;
            signalMask = signalMask >> BitOffsetInByte;
        }
        else
        {
            signalMask =power(2,ComBitSize_copy)-1;
        }
     */

    const ComSignal_type * Signal =  GET_Signal(signalId);
    // Get PDU
    const ComIPdu_type *IPdu = GET_IPdu(Signal->ComIPduHandleId);
    void * const pduBuffer = IPdu->ComIPduDataPtr;

    bitPosition = Signal->ComBitPosition;
    BitOffsetInByte = bitPosition%8;
    pduStartByte = bitPosition / 8;
    pduBufferBytes = (uint8 *)pduBuffer;
    dataBytes = (uint8 *) signalData;
    uint8_t xy= *dataBytes;
    signalLength = Asu_Ceil(Signal->ComBitSize);
    pduBeforChange = pduBufferBytes;
    pduBufferBytes += pduStartByte;
    uint8 x;
    uint8_t ComBitSize_copy =Signal->ComBitSize ;
    boolean IsLessThanOneByte =FALSE ;
    for(i = 0; i<=signalLength; i++)
    {
        // *pduBufferBytes =0xff;
        pduMask = 255;
        signalMask = 255;
        if (IsLessThanOneByte)
            break;
        if( i == 0)
        {
            if( ( ( Signal->ComBitSize ) + (BitOffsetInByte) ) <= 8)
            {
                pduMask=power(2,Signal->ComBitSize)-1;
                pduMask=pduMask<<BitOffsetInByte;
                pduMask =~ pduMask;
                signalMask=power(2,Signal->ComBitSize)-1;
                IsLessThanOneByte =TRUE ;
            }
            else
            {
                pduMask = pduMask >> (8 - BitOffsetInByte);
                signalMask = signalMask >> BitOffsetInByte;
            }
            x=*pduBufferBytes;
            *pduBufferBytes = (* pduBufferBytes) & pduMask;
            x=*pduBufferBytes;
            data = (* dataBytes) & signalMask;
            data = data << BitOffsetInByte;
            *pduBufferBytes = (* pduBufferBytes) | data;
            x= *pduBufferBytes;
            pduBufferBytes ++;
            ComBitSize_copy = ComBitSize_copy - (8-BitOffsetInByte) ;
        }
        else if( ComBitSize_copy<=8 )  //i==signalLength
        {

            pduMask = pduMask << BitOffsetInByte;
            signalMask = signalMask << (8 - BitOffsetInByte);
            x=*pduBufferBytes;
            *pduBufferBytes = (* pduBufferBytes) & pduMask;
            x=*pduBufferBytes;
            data = (* dataBytes) & signalMask;
            data = data >> (8 - BitOffsetInByte);
            *pduBufferBytes = (* pduBufferBytes) | data;
            x= *pduBufferBytes;
            break;
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
            ComBitSize_copy = ComBitSize_copy - 8 ;
        }
    }
}

/*
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
    uint8 i,xx;
    const uint8 *pduBufferBytes = NULL;
    uint8 * signalDataBytes = NULL;

     ComSignal_type * Signal = GET_Signal(signalId);
    const ComIPdu_type *IPdu = GET_IPdu(Signal->ComIPduHandleId);
    const void * pduBuffer = IPdu->ComIPduDataPtr;
    bitPosition = Signal->ComBitPosition;
    signalLength = Asu_Ceil(Signal->ComBitSize);
    BitOffsetInByte = bitPosition % 8;
    pduStartByte = bitPosition / 8;
    dataBytes = (uint8 *) signalData;
    memset(signalData, 0, signalLength);
    pduBufferBytes = (const uint8 *)pduBuffer;
    xx=*pduBufferBytes ;
    pduBufferBytes += pduStartByte;
    uint8_t ComBitSize_copy =Signal->ComBitSize ;
    uint8 x;
    boolean IsLessThanOneByte=FALSE ;
    for(i = 0; i<=signalLength; i++)
    {
        pduMask = 255;
        if (IsLessThanOneByte)
            break;
        if( i == 0)
        {
            if( ( ( Signal->ComBitSize ) + (BitOffsetInByte) ) <= 8)
            {
                pduMask=power(2,Signal->ComBitSize)-1;
                pduMask=pduMask<<BitOffsetInByte;
                IsLessThanOneByte =TRUE ;
            }
            else
            {
                pduMask = pduMask << BitOffsetInByte;
            }
            data = (* pduBufferBytes) & pduMask;
            data = data >> BitOffsetInByte;
            *dataBytes = *dataBytes | data;
            x= *dataBytes;
            pduBufferBytes ++;
            ComBitSize_copy = ComBitSize_copy - (8-BitOffsetInByte) ;
        }
        else if(ComBitSize_copy<=8)
        {
            pduMask = pduMask >> (8-BitOffsetInByte);
            data = (* pduBufferBytes) & pduMask;
            data = data << (8-BitOffsetInByte);
            *dataBytes = (* dataBytes) | data;
            x= *dataBytes;
            break;
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
            ComBitSize_copy = ComBitSize_copy - 8 ;
        }
    }

    /*check if the signal value equal the invalid value or not*/
    /* Don't forget to change the constant value in loop shawqy*/
    uint8_t checked_byte ;
    for(checked_byte = 0 ; checked_byte < 3 ; i++)
    {
        if(dataBytes[checked_byte] == Signal->ComSignalDataInvalidValue[checked_byte])
            continue;
        else
            break;
    }

    if(checked_byte ==2)
        Signal->isInvaildSignalChecked = TRUE;
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

void Com_WriteSignalGrouptoPduBuffer(const Com_SignalGroupIdType SignalGroupId)
{
    /*points to current signal group*/
       const ComSignalGroup_type * SignalGroup = GET_SignalGroup(SignalGroupId);
       /*points to array containing ids of all signals in this group*/
         uint16 *GroupSignals ;
         GroupSignals=SignalGroup->GroupSignals;
       /*number of signals that are in this group*/
        uint8 number_SignalGroup = SignalGroup->number_GroupSignals;
        /*points to data in signalgroup buffer from which data will be copied*/
         void * const SignalGroupBuffer = SignalGroup->ComSignalGroupDataPtr;
         /*casting pointer to data buffer*/
        uint8 * SignalGroupBufferBytes=(uint8 *)SignalGroupBuffer;
        /*buffer mask*/
           uint8 BufferMask;
           /*signal mask*/
           uint8 signalMask;
           /*iterator*/
           uint8 i = 0;
           /*position of signal in the group buffer*/
           uint8 BitOffsetInByte;
            BitOffsetInByte=0;
           uint8 x;
           /*pointer to current signal to be copied*/
           const ComGroupSignal_type * GroupSignal;
           /*size of current signal*/
           uint8 SignalLength ;
           /*pointer to data signal*/
           void * SignalData;
           void *  Data ;
           /*remaining bits to be copied*/
           uint8_t ComBitSize_copy;
           /*flag if signal length is less than byte*/
           boolean IsLessThanOneByte;
           uint8 signalLength_loop;
           uint8 data;
           uint8 j;
           //void *signalData;
           uint8 *dataBytes = (uint8 *) SignalData;

           /*parameter for writing signal to pdu buffer*/

             // Get PDU
          const ComIPdu_type *IPdu;
           void *  pduBuffer;
           uint32 bitPosition;
           uint8 BitOffsetInByteSend ;
            uint8 pduStartByte;
           uint8 *pduBufferBytes=NULL;
           uint8 *pduBeforChange=NULL;
           uint8 xSend;
           uint8_t ComBitSize_copySend ;
           boolean IsLessThanOneByteSend =FALSE ;
           uint8 pduMask;
           uint8 *dataBytesSend = NULL;
           uint8 k;
           uint8 dataSend;
           for (i = 0; i < number_SignalGroup; i++)
           {

               GroupSignal = GET_GroupSignal(GroupSignals[i]);
               SignalLength= GroupSignal->ComBitSize;
               Data = GroupSignal->ComSignalDataPtr;
               /*casting of pointer to data signal*/
                SignalData=(uint8 *)Data;
               ComBitSize_copy =SignalLength ;
                IsLessThanOneByte =FALSE ;
               signalLength_loop = Asu_Ceil(SignalLength);

               IPdu = GET_IPdu(GroupSignal->ComIPduHandleId);
               pduBuffer = IPdu->ComIPduDataPtr;
               bitPosition = GroupSignal->ComBitPosition;
               BitOffsetInByteSend= bitPosition%8;
               pduStartByte = bitPosition / 8;
               pduBufferBytes = (uint8 *)pduBuffer;
               pduBeforChange = pduBufferBytes;
               pduBufferBytes += pduStartByte;
               uint8_t ComBitSize_copy =GroupSignal->ComBitSize ;

               /*inner loop iterator*/
              for(j=0;j<=signalLength_loop;j++)
              {
                          BufferMask = 255;
                          signalMask = 255;
                          if (IsLessThanOneByte)
                          {
                              SignalGroupBufferBytes ++;
                              break;
                          }

                          if( j == 0)
                          {
                              if( ( ( SignalLength ) + (BitOffsetInByte) ) <= 8)
                              {
                                  BufferMask=power(2,SignalLength)-1;
                                  BufferMask=BufferMask<<BitOffsetInByte;
                                  IsLessThanOneByte =TRUE ;
                              }
                              else
                              {
                                  BufferMask = BufferMask<<BitOffsetInByte;
                              }
                              data = (* SignalGroupBufferBytes) & BufferMask;
                              data = data >> BitOffsetInByte;
                              *dataBytes = *dataBytes | data;
                              x= *dataBytes;
                              SignalGroupBufferBytes ++;
                              ComBitSize_copy = ComBitSize_copy - (8-BitOffsetInByte) ;


                          }
                          else if( ComBitSize_copy<=8 )  /*i==signalLength*/
                          {
                              BufferMask = BufferMask >> (8-BitOffsetInByte);
                              data = (* SignalGroupBufferBytes) & BufferMask;
                              data = data << (8-BitOffsetInByte);
                              *dataBytes = (* dataBytes) | data;
                              x= *dataBytes;
                                   break;
                              break;
                          }
                          else
                          {
                              BufferMask = BufferMask >> (8-BitOffsetInByte);
                              data = (* SignalGroupBufferBytes) & BufferMask;
                              data = data << (8-BitOffsetInByte);
                              *dataBytes = (* dataBytes) | data;

                               dataBytes++;

                               BufferMask = 255;
                               BufferMask = BufferMask << BitOffsetInByte;
                               data = (* SignalGroupBufferBytes) & BufferMask;
                               data = data >> BitOffsetInByte;
                               *dataBytes = (* SignalGroupBufferBytes) | data;
                               x= *dataBytes;
                               SignalGroupBufferBytes ++;
                               ComBitSize_copy = ComBitSize_copy - 8 ;
                          }
                      }
              BitOffsetInByte+=(SignalLength%8)+1;

              /*write copied signal to pdu buffer*/

                  for(k = 0; k<=signalLength_loop; k++)
                  {
                      // *pduBufferBytes =0xff;
                      pduMask = 255;
                      signalMask = 255;
                      if (IsLessThanOneByteSend)
                          break;
                      if( k == 0)
                      {
                          if( ( ( bitPosition ) + (BitOffsetInByteSend) ) <= 8)
                          {
                              pduMask=power(2,SignalLength)-1;
                              pduMask=pduMask<<BitOffsetInByteSend;
                              pduMask =~ pduMask;
                              signalMask=power(2,SignalLength)-1;
                              IsLessThanOneByteSend =TRUE ;
                          }
                          else
                          {
                              pduMask = pduMask >> (8 - BitOffsetInByteSend);
                              signalMask = signalMask >> BitOffsetInByteSend;
                          }
                          xSend=*pduBufferBytes;
                          *pduBufferBytes = (* pduBufferBytes) & pduMask;
                          xSend=*pduBufferBytes;
                          dataSend = (x) & signalMask;
                          dataSend = dataSend << BitOffsetInByteSend;
                          *pduBufferBytes = (* pduBufferBytes) | dataSend;
                          xSend= *pduBufferBytes;
                          pduBufferBytes ++;
                          ComBitSize_copySend = ComBitSize_copySend - (8-BitOffsetInByteSend) ;
                      }
                      else if( ComBitSize_copySend<=8 )  //i==signalLength
                      {

                          pduMask = pduMask << BitOffsetInByteSend;
                          signalMask = signalMask << (8 - BitOffsetInByteSend);
                          xSend=*pduBufferBytes;
                          *pduBufferBytes = (* pduBufferBytes) & pduMask;
                          xSend=*pduBufferBytes;
                          dataSend = (x) & signalMask;
                          dataSend = dataSend >> (8 - BitOffsetInByteSend);
                          *pduBufferBytes = (* pduBufferBytes) | dataSend;
                          xSend= *pduBufferBytes;
                          break;
                      }
                      else
                      {
                          pduMask = pduMask << BitOffsetInByteSend;
                          signalMask = signalMask << (8 - BitOffsetInByteSend);
                          *pduBufferBytes = (* pduBufferBytes) & pduMask;
                          dataSend = (x) & signalMask;
                          dataSend = dataSend >> (8 - BitOffsetInByteSend);
                          *pduBufferBytes = (* pduBufferBytes) | dataSend;

                          x++;

                          pduMask = 255;
                          signalMask = 255;
                          pduMask = pduMask >> (8 - BitOffsetInByteSend);
                          signalMask = signalMask >> BitOffsetInByteSend;
                          *pduBufferBytes = (* pduBufferBytes) & pduMask;
                          dataSend = (x) & signalMask;
                          dataSend = dataSend << BitOffsetInByteSend;
                          *pduBufferBytes = (* pduBufferBytes) | dataSend;
                          xSend= *pduBufferBytes;
                          pduBufferBytes ++;
                          ComBitSize_copySend = ComBitSize_copySend - 8 ;
                      }
                  }
              }






}
void Com_PackGroupSignalsToPdu(uint16 ComIPuId)
{
    uint8 groupSignalID = 0;
    const ComIPdu_type *IPdu = GET_IPdu(ComIPuId);
    for ( groupSignalID = 0; (IPdu->ComIPduSignalGroupRef[groupSignalID] != NULL); groupSignalID++)
    {

        Com_SendSignalGroup(IPdu->ComIPduSignalGroupRef[groupSignalID]->ComHandleId);
        Com_WriteSignalGrouptoPduBuffer(IPdu->ComIPduSignalGroupRef[groupSignalID]->ComHandleId);
    }
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



