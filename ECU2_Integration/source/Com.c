/***************************************************
 *
 * File Name: Com.c 
 *
 * Author: AUTOSAR COM Team
 * 
 * Date Created: 6 MAsuh 2019
 * 
 * Version  : 01
 * 
 ****************************************************/
#include "include/Com.h"
#include "include/Com_Types.h"
#include "include/Com_helper.h"
#include "include/Com_Buffer.h"
#include "include/Com_Asu_Types.h"
#include "include/PduR_Com.h"
#include "include/Com_Cbk.h"

/*****************************************************************
 *                     Functions Definitions                     *
 *****************************************************************/


/* Com_Config declaration*/
const ComConfig_type * ComConfig;
uint8 com_pdur[] = {vcom};

/* Com_Asu_Config declaration*/
extern Com_Asu_Config_type ComAsuConfiguration;
static Com_Asu_Config_type * Com_Asu_Config = &ComAsuConfiguration;


void Com_Init( const ComConfig_type* config)
{
    /* Initialize ComConfig */
    ComConfig = config;
    const ComSignal_type *Signal;

    //1- loop on IPDUs
    uint16 pduId;
    for ( pduId = 0; pduId<COM_NUM_OF_IPDU; pduId++) {

        // 1.1- Initialize I-PDU
        const ComIPdu_type *IPdu = GET_IPdu(pduId);
        Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(pduId);

        Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeRepetitionPeriodTimer = \
                IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeRepetitionPeriod;

        Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeTimePeriodTimer = \
                IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeTimePeriod;

        Asu_IPdu->Com_Asu_Pdu_changed = FALSE;

        Asu_IPdu->Com_Asu_First_Repetition = TRUE;


        // Initialize the memory with the default value.
        if (IPdu->ComIPduDirection == SEND) {
            memset((void *)IPdu->ComIPduDataPtr, IPdu->ComTxIPdu.ComTxIPduUnusedAreasDefault, IPdu->ComIPduSize);
        }

        // For each signal in this PDU
        uint16 signalId;
        for ( signalId = 0; (IPdu->ComIPduSignalRef != NULL) && \
        (IPdu->ComIPduSignalRef[signalId] != NULL); signalId++)
        {
            Signal = IPdu->ComIPduSignalRef[signalId];

            // Clear update bits
            if(Signal->isUpdateBitUsed==TRUE)
                CLEARBIT(IPdu->ComIPduDataPtr, Signal->ComUpdateBitPosition);
        }

    }
}

static void Com_RxProcessSignals(PduIdType ComRxPduId)
{
    const ComSignal_type *comSignal = NULL;
    const ComIPdu_type *IPdu = GET_IPdu(ComRxPduId);
    Com_Asu_Signal_type * Asu_Signal = NULL;
    uint8 signalId;
    for( signalId = 0; IPdu->ComIPduSignalRef[signalId] != NULL; signalId++)
    {
        comSignal = IPdu->ComIPduSignalRef[signalId];
        Asu_Signal = GET_AsuSignal(comSignal->ComHandleId);
        if(comSignal->isUpdateBitUsed==TRUE)
        {
            if (CHECKBIT(IPdu->ComIPduDataPtr, comSignal->ComUpdateBitPosition))
            {
                if (IPdu->ComIPduSignalProcessing == IMMEDIATE)
                {
                    // unpack the pdu and update signal buffer
                    Com_UnPackSignalsFromPdu(ComRxPduId);

                    // If signal processing mode is IMMEDIATE, notify the signal callback.
                    if (IPdu->ComIPduSignalRef[signalId]->ComNotification != NULL)
                    {
                        IPdu->ComIPduSignalRef[signalId]->ComNotification();
                    }
                }
                else
                {
                    // Signal processing mode is DEFERRED, mark the signal as updated.
                    Asu_Signal->ComSignalUpdated = 1;
                }
            }
        }
    }
}

void Com_MainFunctionRx(void)
{
    const ComIPdu_type *IPdu = NULL;
    ComSignal_type *signal = NULL;
    Com_Asu_Signal_type * Asu_Signal = NULL;
    Com_Asu_IPdu_type *Asu_IPdu = NULL;
    uint16 signalID;
    boolean pduUpdated = FALSE;
    uint8_t jj;
    uint8_t local_arr[3];


    /* Loop on IPDUs */
    uint16 pduId;
    for ( pduId = 0; pduId < COM_NUM_OF_IPDU; pduId++)
    {
        IPdu = GET_IPdu(pduId);
        Asu_IPdu = GET_AsuIPdu(pduId);

        /*
         * if the PDU is send then skip this this PDU*/
        if(!(IPdu->ComIPduDirection == RECEIVE))
            continue;

        for (signalID = 0; (IPdu->ComIPduSignalRef != NULL) && (IPdu->ComIPduSignalRef[signalID] != NULL); signalID++)
        {
            signal = IPdu->ComIPduSignalRef[signalID];
            if (Asu_Signal->ComSignalUpdated||signal->isUpdateBitUsed == FALSE)
            {
                pduUpdated = TRUE;
            }
        }

        if (pduUpdated && IPdu->ComIPduSignalProcessing == DEFERRED && IPdu->ComIPduDirection == RECEIVE && IPdu->ComIPduType == NORMAL)
        {
            /* unlock the buffer */
            UNLOCKBUFFER(&Asu_IPdu->PduBufferState)

                                                                                    /* copy the deferred buffer to the actual pdu buffer */
                                                                                    Com_UnPackSignalsFromPdu(pduId);

            /* loop on the signal in this ipdu */
            for (signalID = 0; (IPdu->ComIPduSignalRef != NULL) && (IPdu->ComIPduSignalRef[signalID] != NULL); signalID++)
            {
                signal = IPdu->ComIPduSignalRef[signalID];
                Asu_Signal = GET_AsuSignal(signal->ComHandleId);

                if(TRUE==signal->isInvaildSignalChecked)
                {
                    switch(signal->ComDataInvalidAction)
                    {
                    case NOTIFY:
                        if (signal->ComInvalidNotification != NULL)
                        {
                            signal->ComInvalidNotification();
                        }
                        break;

                    case REPLACE:
                        /*this local array to copy the data of initial value
                         * value of the signal to the data signal pointer
                         * */
                        for(jj = 0 ; jj<3; jj++)
                        {
                            local_arr[jj] = signal->ComSignalInitValue[jj];
                        }
                        signal->ComSignalDataPtr=local_arr;

                        /* if at least on signal is Updated, mark this Pdu as Updated */
                        if (Asu_Signal->ComSignalUpdated||signal->isUpdateBitUsed== FALSE)
                        {
                            if (signal->ComNotification != NULL)
                            {
                                signal->ComNotification();
                            }
                            Asu_Signal->ComSignalUpdated = FALSE;
                        }
                        break;
                    }
                }


            }
        }
    }
}

void Com_MainFunctionTx(void)
{
    const ComIPdu_type *IPdu;
    Com_Asu_IPdu_type *Asu_IPdu;
    boolean mixed_t;
    boolean mixedSent;
    boolean periodicDirect=FALSE;

    //Loop on IPDUs
    uint16 pduId;
    for ( pduId = 0; pduId<COM_NUM_OF_IPDU; pduId++)
    {
        IPdu = GET_IPdu(pduId);
        Asu_IPdu = GET_AsuIPdu(pduId);
        periodicDirect=FALSE;

        /* if it is a send PDU*/
        if(IPdu->ComIPduDirection == SEND)
        {
            mixed_t = FALSE;

            switch(IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeMode)
            {
            /* if the transmission mode is mixed */
            case MIXED:
                mixed_t = TRUE;
                /* no break because the mixed is periodic and direct */
                /* if the transmission mode is periodic */
            case PERIODIC:
                if(mixed_t)
                    periodicDirect=TRUE;

                timerDec(Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeTimePeriodTimer);

                if(Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeTimePeriodTimer<=0)
                {
                    if(Com_TriggerIPDUSend(pduId) == E_OK)
                    {
                        Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeTimePeriodTimer = \
                                IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeTimePeriod;
                    }
                }
                if(!mixed_t)/* in case the Pdu is mixed don't break */
                    break;
                /* if the transmission mode is direct */
            case DIRECT:
                if(Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft >= 0)
                {

                    timerDec(Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeRepetitionPeriodTimer);
                    /*    if(mixed_t && periodicDirect)
                        break;*/

                    if(Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeRepetitionPeriodTimer <= 0 || Asu_IPdu->Com_Asu_First_Repetition )
                    {
                        if(Com_TriggerIPDUSend(pduId)== E_OK)
                        {
                            Asu_IPdu->Com_Asu_TxIPduTimers.ComTxModeRepetitionPeriodTimer = \
                                    IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeRepetitionPeriod;

                            Asu_IPdu->Com_Asu_First_Repetition = FALSE;

                            Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft --;
                        }
                    }
                }
            }
        }
    }
}

/* Updates the signal object identified by SignalId with the signal referenced by the SignalDataPtr parameter */
uint8 Com_SendSignal( Com_SignalIdType SignalId, const void* SignalDataPtr )
{
    /* validate signalID */
    if(!validateSignalID(SignalId) )
        return E_NOT_OK;

    /* Get signal of "SignalId" */
    const ComSignal_type *Signal = GET_Signal(SignalId);


    /*Get IPdu of this signal */
    const ComIPdu_type *IPdu = GET_IPdu(Signal->ComIPduHandleId);

    /* Get IPDU_Asu of signal ipduHandleId */
    Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(Signal->ComIPduHandleId);
    uint32 xd=(uint32 *)SignalDataPtr;

    /* update the Signal buffer with the signal data */
    Com_WriteSignalDataToSignalBuffer(Signal->ComHandleId, SignalDataPtr);

    switch(Signal->ComTransferProperty)
    {
    case TRIGGERED_WITHOUT_REPETITION:
        Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = 1;
        break;

    case TRIGGERED:
        Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = \
        (IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeNumberOfRepetitions) + 1;
        break;

    case TRIGGERED_ON_CHANGE:
        if (Asu_IPdu->Com_Asu_Pdu_changed)
        {
            Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = \
                    (IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeNumberOfRepetitions) + 1;
            Asu_IPdu->Com_Asu_Pdu_changed = FALSE;
        }
        break;

    case TRIGGERED_ON_CHANGE_WITHOUT_REPETITION:
        if (Asu_IPdu->Com_Asu_Pdu_changed)
        {
            Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = 1;
            Asu_IPdu->Com_Asu_Pdu_changed = FALSE;
        }
    }

    if(Signal->isUpdateBitUsed==TRUE)
    {
        /* Set the update bit of this signal */
        SETBIT(IPdu->ComIPduDataPtr, Signal->ComUpdateBitPosition);
    }
    uint8 x;
    uint8 i;
    for ( i =0; i<8; i++)
    {
        x = *(uint8 *)((uint8 *)IPdu->ComIPduDataPtr + i);
    }

    Asu_IPdu->Com_Asu_First_Repetition = TRUE;


    return E_OK;

}

uint8 Com_SendSignalv2( Com_SignalGroupIdType SignalId, const void* SignalDataPtr )
{
    /* validate signalID */
    if(!validateGroupSignalIDv2(SignalId) )
        return E_NOT_OK;

    /* Get signal of "SignalId" */
    const ComGroupSignal_type *groupSignal = GET_GroupSignal(SignalId);
    /*Get IPdu of this signal */
    const ComIPdu_type *IPdu = GET_IPdu(groupSignal->ComIPduHandleId);

    /* Get IPDU_Asu of signal ipduHandleId */
    Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(groupSignal->ComIPduHandleId);
    uint32 xd=(uint32 *)SignalDataPtr;
    Com_WritegroupSignalDataToSignalBuffer(groupSignal->ComHandleId, SignalDataPtr);

    /* update the Signal buffer with the signal data */

    switch(groupSignal->ComTransferProperty)
    {
    case TRIGGERED_WITHOUT_REPETITION:
        Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = 1;
        break;

    case TRIGGERED:
        Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = \
        (IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeNumberOfRepetitions) + 1;
        break;

    case TRIGGERED_ON_CHANGE:
        if (Asu_IPdu->Com_Asu_Pdu_changed)
        {
            Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = \
                    (IPdu->ComTxIPdu.ComTxModeFalse.ComTxMode.ComTxModeNumberOfRepetitions) + 1;
            Asu_IPdu->Com_Asu_Pdu_changed = FALSE;
        }
        break;

    case TRIGGERED_ON_CHANGE_WITHOUT_REPETITION:
        if (Asu_IPdu->Com_Asu_Pdu_changed)
        {
            Asu_IPdu->Com_Asu_TxIPduTimers.ComTxIPduNumberOfRepetitionsLeft = 1;
            Asu_IPdu->Com_Asu_Pdu_changed = FALSE;
        }
    }


    uint8 x;
    uint8 i;
    for ( i =0; i<8; i++)
    {
        x = *(uint8 *)((uint8 *)IPdu->ComIPduDataPtr + i);
    }

    Asu_IPdu->Com_Asu_First_Repetition = TRUE;


    return E_OK;

}



/* Copies the data of the signal identified by SignalId to the location specified by SignalDataPtr */
uint8 Com_ReceiveSignal( Com_SignalIdType SignalId, void* SignalDataPtr )
{
    /* validate signalID */
    if(!validateSignalID(SignalId) )
        return E_NOT_OK;

    /* Get signal of "SignalId" */
    const ComSignal_type *Signal = GET_Signal(SignalId);

    /* Get IPDU of signal ipduHandleId */
    const ComIPdu_type *IPdu = GET_IPdu(Signal->ComIPduHandleId);

    /* check ipdu direction is receive */
    if(IPdu->ComIPduDirection == RECEIVE)
    {
        Com_ReadSignalDataFromSignalBuffer(SignalId, SignalDataPtr);
    }
    else
    {
        return E_NOT_OK;
    }
    return E_OK;
}

BufReq_ReturnType Com_CopyTxData( PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr )
{
    ComIPdu_type *IPdu = GET_IPdu(id);
    Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(id);

    if( (IPdu->ComIPduDirection == SEND) &&
            (Asu_IPdu->PduBufferState.CurrentPosition + info->SduLength <= IPdu->ComIPduSize) )
    {
        void * source = (uint8*)IPdu->ComIPduDataPtr + Asu_IPdu->PduBufferState.CurrentPosition;
        LOCKBUFFER(&Asu_IPdu->PduBufferState);
        memcpy( (void*) info->SduDataPtr, source, info->SduLength);
        Asu_IPdu->PduBufferState.CurrentPosition += info->SduLength;
        *availableDataPtr = IPdu->ComIPduSize - Asu_IPdu->PduBufferState.CurrentPosition;
        return BUFREQ_OK;
    }
    else
    {
        return BUFREQ_E_NOT_OK;
    }

}

BufReq_ReturnType Com_CopyRxData( PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr )
{
    ComIPdu_type *IPdu = GET_IPdu(id);
    Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(id);

    if( (IPdu->ComIPduDirection == RECEIVE) &&\
            (IPdu->ComIPduSize - Asu_IPdu->PduBufferState.CurrentPosition >= info->SduLength )&&\
            Asu_IPdu->PduBufferState.Locked)
    {
        void* distination =(void*)((uint8 *) IPdu->ComIPduDataPtr+ Asu_IPdu->PduBufferState.CurrentPosition);
        if(info->SduDataPtr != NULL)
            memcpy(distination, info->SduDataPtr, info->SduLength);
        Asu_IPdu->PduBufferState.CurrentPosition += info->SduLength;
        *bufferSizePtr = IPdu->ComIPduSize - Asu_IPdu->PduBufferState.CurrentPosition;

        return BUFREQ_OK;
    }
    else
    {
        return BUFREQ_E_NOT_OK;
    }
}

Std_ReturnType Com_TriggerIPDUSend( PduIdType PduId )
{
    const ComIPdu_type *IPdu = GET_IPdu(PduId);
    Com_Asu_IPdu_type *Asu_IPdu = GET_AsuIPdu(PduId);
    PduInfoType PduInfoPackage;
    uint8 signalID;

    //Com_PackSignalsToPdu(PduId);
    Com_PackGroupSignalsToPdu(PduId);

    PduInfoPackage.SduDataPtr = (uint8 *)IPdu->ComIPduDataPtr;
    PduInfoPackage.SduLength = IPdu->ComIPduSize;
    uint8 i;
    uint8 x;
    for ( i =0; i<PduInfoPackage.SduLength; i++)
    {
        x = *(uint8 *)((uint8 *)PduInfoPackage.SduDataPtr + i);
    }

    if (Asu_IPdu->PduBufferState.Locked)
    {
        return E_NOT_OK;
    }

    if (PduR_ComTransmit(com_pdur[IPdu->ComIPduHandleId], &PduInfoPackage) == E_OK)
    {
        // Clear all update bits for the contained signals
        if(IPdu->ComTxIPdu.ComTxIPduClearUpdateBit == TRANSMIT)
        {
            for ( signalID = 0; (IPdu->ComIPduSignalRef[signalID] != NULL); signalID++)
            {
                if(IPdu->ComIPduSignalRef[signalID]->isUpdateBitUsed==TRUE)
                    CLEARBIT(IPdu->ComIPduDataPtr, IPdu->ComIPduSignalRef[signalID]->ComUpdateBitPosition);
            }
        }

    }
    else
    {
        return E_NOT_OK;
    }
    return E_OK;
}

void Com_RxIndication(PduIdType ComRxPduId, const PduInfoType* PduInfoPtr)
{
    const ComIPdu_type *IPdu = GET_IPdu(ComRxPduId);

    memcpy(IPdu->ComIPduDataPtr, PduInfoPtr->SduDataPtr, IPdu->ComIPduSize);

    Com_RxProcessSignals(ComRxPduId);

    return;
}

BufReq_ReturnType Com_StartOfReception(PduIdType id,const PduInfoType *info,PduLengthType TpSduLength,PduLengthType *bufferSizePtr)
{
    Com_Asu_IPdu_type *AsuIPdu=GET_AsuIPdu(id);
    PduLengthType ComIPduSize;

    if(GET_IPdu(id)->ComIPduDirection==RECEIVE && GET_IPdu(id)->ComIPduType == TP)
    {
        //making sure that the buffer is unlocked
        if(!AsuIPdu->PduBufferState.Locked)
        {
            //getting the ipdu size
            ComIPduSize = GET_IPdu(id)->ComIPduSize;
            //making sure that we have the enough space for the sdu
            if(ComIPduSize>=TpSduLength)
            {
                //lock the buffer until copying is done
                LOCKBUFFER(&AsuIPdu->PduBufferState);
                ///return the available buffer size
                *bufferSizePtr=ComIPduSize;
            }
            else
            {
                return BUFREQ_E_OVFL;
            }
        }
        else
        {
            return BUFREQ_E_BUSY;
        }
        return BUFREQ_OK;
    }
    return BUFREQ_E_NOT_OK;
}



void Com_TpRxIndication(PduIdType id,Std_ReturnType Result)
{
    const ComIPdu_type *ipdu=GET_IPdu(id);
    Com_Asu_IPdu_type *AsuIPdu=GET_AsuIPdu(id);

    if (Result == E_OK)
    {
        if (ipdu->ComIPduSignalProcessing == IMMEDIATE)
        {
            UNLOCKBUFFER(&AsuIPdu->PduBufferState);

            // In deferred mode, buffers are unlocked in mainfunction
            Com_RxProcessSignals(id);
        }
    }
    else
    {
        UNLOCKBUFFER(&AsuIPdu->PduBufferState);
    }
}


void Com_TpTxConfirmation(PduIdType PduId, Std_ReturnType Result)
{
    uint8 signalId;
    ComSignal_type * signal = NULL;
    ComIPdu_type *ipdu=GET_IPdu(PduId);
    Com_Asu_IPdu_type *AsuIPdu=GET_AsuIPdu(PduId);

    UNLOCKBUFFER(&AsuIPdu->PduBufferState);

    if (ipdu->ComTxIPdu.ComTxIPduClearUpdateBit == CONFIRMATION)
    {
        for(signalId = 0; (ipdu->ComIPduSignalRef != NULL) && (ipdu->ComIPduSignalRef[signalId] != NULL) ; signalId++)
        {
            signal = ipdu->ComIPduSignalRef[signalId];
            if(signal->isUpdateBitUsed==TRUE)
                CLEARBIT(ipdu->ComIPduDataPtr,signal->ComUpdateBitPosition);
        }
    }
}


/*****************************************************************
==============================Description=====================================================
The service Com_InvalidateSignal invalidates the signal with the given SignalId by
setting its value to its configured ComSignalDataInvalidValue.
==============================parameters======================================================
IN: SignalId
OUT:None
 */
uint8 Com_InvalidateSignal(Com_SignalIdType SignalId)
{
    ComSignal_type *Signal = GET_Signal(SignalId);


    if(TRUE==Signal->isInvaildSignalUsed)
       return  Com_SendSignal(  SignalId, Signal->ComSignalDataInvalidValue);
    else
       return  Com_SendSignal(  SignalId, Signal->ComSignalDataPtr);
}



/* This function copies group signal from the shadow buffer to signal group buffer*/
uint8 Com_SendSignalGroup(Com_SignalGroupIdType SignalGroupId)
{
    /*loop over each groupsignal belonging to that signalgroupid*/
    /*points to current signal group*/
    const ComSignalGroup_type * SignalGroup = GET_SignalGroup(SignalGroupId);
    /*points to array containing ids of all signals in this group*/
    uint16 *GroupSignals ;
    GroupSignals=SignalGroup->GroupSignals;
    /*number of signals that are in this group*/
    uint8 number_SignalGroup = SignalGroup->number_GroupSignals;
    /*points to data in signalgroup buffer to which data will be copied*/
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
    uint8 * SignalData;
    void *  Data ;
    /*remaining bits to be copied*/
    uint8_t ComBitSize_copy;
    /*flag if signal length is less than byte*/
    boolean IsLessThanOneByte,isonebyte=FALSE;
    uint8 signalLength_loop;
    uint8 data;
    uint8 j;

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


        /*inner loop iterator*/
       for(j=0;j<=signalLength_loop;j++)
       {
                   BufferMask = 255;
                   signalMask = 255;
                   if (IsLessThanOneByte)
                   {
                      if( ! isonebyte) SignalGroupBufferBytes --;
                       break;
                   }

                   if( j == 0)
                   {
                       if( ( ( SignalLength ) + (BitOffsetInByte) ) <= 8)
                       {
                           BufferMask=power(2,SignalLength)-1;
                           BufferMask=BufferMask<<BitOffsetInByte;
                           BufferMask =~ BufferMask;
                           signalMask=power(2,SignalLength)-1;
                           IsLessThanOneByte =TRUE ;
                           if( ( ( SignalLength ) + (BitOffsetInByte) ) == 8)
                               isonebyte=TRUE;

                       }
                       else
                       {
                           BufferMask = BufferMask >> (8 - BitOffsetInByte);
                           signalMask = signalMask >> BitOffsetInByte;
                       }
                       x=*SignalGroupBufferBytes;
                       *SignalGroupBufferBytes = (* SignalGroupBufferBytes) & BufferMask;
                       x=*SignalGroupBufferBytes;
                       data = (* SignalData) & signalMask;
                       data = data << BitOffsetInByte;
                       *SignalGroupBufferBytes = (* SignalGroupBufferBytes) | data;
                       x= *SignalGroupBufferBytes;
                       SignalGroupBufferBytes ++;
                       ComBitSize_copy = ComBitSize_copy - (8-BitOffsetInByte) ;
                   }
                   else if( ComBitSize_copy<=8 )  /*i==signalLength*/
                   {

                       BufferMask = BufferMask << BitOffsetInByte;
                       signalMask = signalMask << (8 - BitOffsetInByte);
                       x=*SignalGroupBufferBytes;
                       *SignalGroupBufferBytes = (* SignalGroupBufferBytes) & BufferMask;
                       x=*SignalGroupBufferBytes;
                       data = (*SignalData) & signalMask;
                       data = data >> (8 - BitOffsetInByte);
                       *SignalGroupBufferBytes = (* SignalGroupBufferBytes) | data;
                       x= *SignalGroupBufferBytes;
                       break;
                   }
                   else
                   {
                       BufferMask = BufferMask << BitOffsetInByte;
                       signalMask = signalMask << (8 - BitOffsetInByte);
                       *SignalGroupBufferBytes = (* SignalGroupBufferBytes) & BufferMask;
                       data = (* SignalData) & signalMask;
                       data = data >> (8 - BitOffsetInByte);
                       *SignalGroupBufferBytes = (* SignalGroupBufferBytes) | data;

                       SignalData++;

                       BufferMask = 255;
                       signalMask = 255;
                       BufferMask = BufferMask >> (8 - BitOffsetInByte);
                       signalMask = signalMask >> BitOffsetInByte;
                       *SignalGroupBufferBytes = (* SignalGroupBufferBytes) & BufferMask;
                       data = (* SignalData) & signalMask;
                       data = data << BitOffsetInByte;
                       *SignalGroupBufferBytes= (* SignalGroupBufferBytes) | data;
                       x= *SignalGroupBufferBytes;
                       SignalGroupBufferBytes++;
                       ComBitSize_copy = ComBitSize_copy - 8 ;
                   }
               }
       BitOffsetInByte+=(SignalLength%8);
       }



    return E_OK;
}
