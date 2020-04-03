
/***************************************************
 *
 * File Name: Com_cfg.c 

 *
 * Author: AUTOSAR COM Team 
 * 
 * Date Created: 13 March 2019
 * 
 * Version  : 01
 * 
 ****************************************************/

#include "include/Std_Types.h"
#include "include/Com_Types.h"
#include "include/Com_Cfg.h"

/* signals Buffer */
uint8 ComSignalBuffer_1 [3];
uint8 ComSignalBuffer_2 [3];
uint8 ComSignalBuffer_12 [3];
uint8 ComSignalBuffer_13 [3];
uint8 ComSignalBuffer_14 [3];
uint8 ComSignalBuffer_3 [3];
uint8 ComSignalBuffer_4 [3];
uint8 ComSignalgroupBuffer_1 [3];
uint16 GroupSignal[3]={0,1,2} ;
//uint8 ComSignalBuffer_4 [2];
//uint8 ComSignalBuffer_5 [1];
//uint8 ComSignalBuffer_6 [4];

/*buffer to save invalid value*/
uint8 InvalidValueBuffer[3] ={0x07};

/*buffer to save initial value*/
uint8 InitialValue[3] = {0};

/*this function is to be written in RTE
 * assuming this function assigned to signal0 */
extern void com_notification(void);

/*this function is to be written in RTE
 * assuming this function assigned to signal1 */
extern void com_notification_1(void);


ComGroupSignal_type comgroupsignal[]=
{
 {
 //signal0
  .ComBitPosition= 0,
  .ComHandleId= 0 ,
  .ComBitSize= 8,
  .ComSignalDataPtr = ComSignalBuffer_12,
  .ComSignalType = UINT32,
  .ComTransferProperty = TRIGGERED_ON_CHANGE,
  .ComIPduHandleId=1,
  .IsGroupSignal=TRUE,
 },
 {
  //signal1
   .ComBitPosition= 9,
   .ComHandleId= 1 ,
   .ComBitSize= 8,
   .ComSignalDataPtr = ComSignalBuffer_13,
   .ComSignalType = UINT32,
   .ComTransferProperty = TRIGGERED_ON_CHANGE,
   .ComIPduHandleId=1,
   .IsGroupSignal=TRUE,
 },
 {
  //signal2
   .ComBitPosition= 18,
   .ComHandleId= 2 ,
   .ComBitSize= 8,
   .ComSignalDataPtr = ComSignalBuffer_14,
   .ComSignalType = UINT32,
   .ComTransferProperty = TRIGGERED_ON_CHANGE,
   .ComIPduHandleId=1,
   .IsGroupSignal=TRUE,
 }
};

ComSignalGroup_type comsignalgroup[]=
                                   {
                                    {
                                    .ComHandleId=0,
                                    .ComUpdateBitPosition=25,
                                    .ComTransferProperty= TRIGGERED_ON_CHANGE,
                                    .ComSignalGroupDataPtr=ComSignalgroupBuffer_1,
                                    .GroupSignals=GroupSignal,
                                    .number_GroupSignals=3,
                                    .ComIPduHandleId=1
                                    }
                                    };

 const ComSignal_type ComSignal[] =
{
 {  //signal0
    .ComBitPosition= 0,
    .ComUpdateBitPosition= 42 ,
    .ComHandleId= 0 ,
    .ComBitSize= 8,
    .ComSignalType = BOOLEAN,
    .ComTransferProperty = TRIGGERED_ON_CHANGE,
    .ComIPduHandleId=0,
    .ComNotification=NULL,
    .ComSignalDataPtr = ComSignalBuffer_1
 },
 {  //signal1
    .ComBitPosition= 50,
    .ComUpdateBitPosition= 60,
    .ComHandleId= 1 ,
    .ComBitSize= 8,
    .ComSignalType = BOOLEAN,
    .ComTransferProperty = TRIGGERED_ON_CHANGE,
    .ComIPduHandleId=0,
    .ComNotification=NULL,
    .ComSignalDataPtr = ComSignalBuffer_2,
    .ComSignalDataInvalidValue=InvalidValueBuffer,
    .isInvaildSignalUsed=TRUE,
    .isInvaildSignalChecked = TRUE,
    .ComSignalInitValue =InitialValue,
    .ComDataInvalidAction = REPLACE , //   REPLACE NOTIFY
    .ComInvalidNotification =&com_notification_1
 },
 {  //signal2
    .ComBitPosition= 50,
    .ComUpdateBitPosition= 58 ,
    .ComHandleId= 2 ,
    .ComBitSize= 8,
    .ComSignalType = UINT8,
    .ComTransferProperty = TRIGGERED_ON_CHANGE,
    .ComIPduHandleId=1,
    .ComNotification=NULL,
    .ComSignalDataPtr = ComSignalBuffer_3,
    .isUpdateBitUsed=TRUE
 },
 {   //signal3
     .ComBitPosition= 40,
     .ComUpdateBitPosition= 48 ,
     .ComHandleId= 3 ,
     .ComBitSize= 8,
     .ComSignalType = UINT8,
     .ComTransferProperty = TRIGGERED_ON_CHANGE,
     .ComIPduHandleId=1,
     .ComNotification=NULL,
     .ComSignalDataPtr = ComSignalBuffer_4,
     .isUpdateBitUsed=TRUE,
     .ComSignalDataInvalidValue=InvalidValueBuffer,
     .isInvaildSignalUsed=TRUE,
     .isInvaildSignalChecked = FALSE,
     .ComSignalInitValue =InitialValue,
     .IsGroupSignal=FALSE
 },
 /* {   //signal0
        .ComBitPosition= 0,
        .ComUpdateBitPosition= 16 ,
        .ComHandleId= 3 ,
        .ComBitSize= 16,
        .ComSignalType = UINT16,
        .ComTransferProperty = TRIGGERED_ON_CHANGE,
        .ComIPduHandleId=1,
        .ComSignalDataPtr = ComSignalBuffer_4
    },
    {   //signal1
        .ComBitPosition= 17,
        .ComUpdateBitPosition= 25,
        .ComHandleId= 4 ,
        .ComBitSize= 8,
        .ComSignalType = BOOLEAN,
        .ComTransferProperty = TRIGGERED_ON_CHANGE,
        .ComIPduHandleId=1,
        .ComNotification=NULL,
        .ComSignalDataPtr = ComSignalBuffer_5
    },
    {   //signal2
        .ComBitPosition= 26,
        .ComUpdateBitPosition= 58 ,
        .ComHandleId= 5 ,
        .ComBitSize= 32,
        .ComSignalType = UINT32,
        .ComTransferProperty = TRIGGERED_ON_CHANGE,
        .ComIPduHandleId=1,
        .ComNotification=NULL,
        .ComSignalDataPtr = ComSignalBuffer_6
    },
    {   //signal3
        .ComBitPosition= 0,
        .ComUpdateBitPosition= 16 ,
        .ComHandleId= 6 ,
        .ComBitSize= 16,
        .ComSignalType = UINT16,
        .ComTransferProperty = TRIGGERED,
        .ComIPduHandleId=2,
        .ComNotification=NULL,
        .ComSignalDataPtr = ComSignalBuffer_6

    },
    {   //signal4
        .ComBitPosition= 17,
        .ComUpdateBitPosition= 33 ,
        .ComHandleId= 4 ,
        .ComBitSize= 16,
        .ComSignalType = UINT16,
        .ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
        .ComIPduHandleId=1,
        .ComNotification=NULL,
        .ComSignalDataPtr = ComSignalBuffer_5
    },
    {   //signal5
        .ComBitPosition= 0,
        .ComUpdateBitPosition= 64 ,
        .ComHandleId= 5,
        .ComBitSize= 64,
        .ComSignalType = UINT64,
        .ComTransferProperty = TRIGGERED_ON_CHANGE_WITHOUT_REPETITION,
        .ComIPduHandleId=2,
        .ComNotification=NULL,
        .ComSignalDataPtr = ComSignalBuffer_6
    },*/

};

/* IPdu signal lists. */
const ComSignal_type * const ComIPduSignalRefs_Can_Message_1[] = {
                                                                     &ComSignal[ passengeronright ], //
                                                                      &ComSignal[ passengeronleft ],//
                                                                  //&ComSignal[ seatstatusright ],
                                                                  NULL
};

const ComSignal_type * const ComIPduSignalRefs_Can_Message_2[] = {
                                                                //   &ComSignal[ passengeronright ], //
                                                                  // &ComSignal[ passengeronleft ],//
                                                                  &comsignalgroup[0],
                                                                  // &ComSignal[ CanDB_Signal_29_12_BE_Tester],
                                                                  // &ComSignal[ CanDB_Signal_1_4_LE ],
                                                                  NULL
};
const ComSignal_type * const ComIPduSignalRefs_Can_Message_0[]={
                                                                &ComSignal[ 0 ],//
                                                                &ComSignal[ 1 ]
                                                                };
const ComSignalGroup_type * const Can_Message_0[]=
{
 &comsignalgroup[0],
 NULL
};
//const ComSignal_type * const ComIPduSignalRefs_Can_Message_3[] = {
//    &ComSignal[ CanDB_Signal_1_4_LE ],
//    NULL
//};
/* IPdu buffers and signal */
uint8 ComIPduBuffer_1[3];

//uint8 ComIPduBuffer_11[3]={0x07,0x03,0xff};

uint8 ComIPduBuffer_2[10];

//uint8 ComIPduBuffer_3[8];

/* Com Ipdu */
const ComIPdu_type ComIPdu[] = 
{
 { // CanDB_Message_1
   .ComIPduDirection = RECEIVE ,
   .ComIPduSize=8,
   .ComIPduHandleId = 0 ,
   .ComIPduSignalRef =ComIPduSignalRefs_Can_Message_0,
   .ComIPduDataPtr=ComIPduBuffer_1,
   .ComIPduType = NORMAL,
   .ComTxIPdu =
   {
    .ComTxModeFalse =
    {
     .ComTxMode=
     {
      .ComTxModeMode = PERIODIC,
      .ComTxModeNumberOfRepetitions = 2,
      .ComTxModeTimePeriod=200,
      .ComTxModeRepetitionPeriod=200,
     }
    },

    .ComMinimumDelayTime = 0,
    .ComTxIPduClearUpdateBit = TRANSMIT,
    .ComTxIPduUnusedAreasDefault = 0,

   }
 },
 { // CanDB_Message_2
   .ComIPduDirection = SEND ,
   .ComIPduSize=8,
   .ComIPduSignalProcessing = IMMEDIATE,
   .ComIPduHandleId = 1 ,
   .ComIPduSignalRef = ComIPduSignalRefs_Can_Message_1,
   .ComIPduSignalGroupRef=ComIPduSignalRefs_Can_Message_2,
   .ComIPduDataPtr=ComIPduBuffer_2,
   .ComIPduType = NORMAL,
   .ComTxIPdu =
   {
    .ComTxModeFalse =
    {
     .ComTxMode=
     {
      .ComTxModeMode = DIRECT,
      .ComTxModeNumberOfRepetitions = 10,
      .ComTxModeTimePeriod=500,
      .ComTxModeRepetitionPeriod=200
     }
    },
    .ComMinimumDelayTime = 0,
    .ComTxIPduUnusedAreasDefault = 0,
   }
 },
 /* { // CanDB_Message_2
          .ComIPduDirection = SEND ,
           .ComIPduSize=8,
          .ComIPduHandleId = 2 ,
          .ComIPduSignalRef =ComIPduSignalRefs_Can_Message_3,
          .ComIPduDataPtr=ComIPduBuffer_3,
          .ComIPduType = TP,
          .ComTxIPdu =
          {
               .ComTxModeFalse =
               {
                    .ComTxMode=
                    {
                      .ComTxModeMode = MIXED,
                      .ComTxModeNumberOfRepetitions = 2,
                      .ComTxModeTimePeriod = 100,
                      .ComTxModeRepetitionPeriod = 1000
                    }
               },

               .ComMinimumDelayTime = 0,
               .ComTxIPduUnusedAreasDefault = 0,

          }
      },
      { // CanDB_Message_3
            .ComIPduDirection = SEND ,
            .ComIPduSize=8,
            .ComIPduHandleId = 3 ,
            .ComIPduSignalRef =ComIPduSignalRefs_Can_Message_3,
            .ComIPduDataPtr=ComIPduBuffer_3,
            .ComTxIPdu =
            {
                 .ComTxModeFalse =
                 {
                      .ComTxMode=
                      {
                        .ComTxModeMode = NONE,
                        .ComTxModeNumberOfRepetitions = 2,
                        .ComTxModeTimePeriod = 10,
                        .ComTxModeRepetitionPeriod = 100
                      }
                 },

                 .ComMinimumDelayTime = 0,
                 .ComTxIPduUnusedAreasDefault = 0,

            }
        }*/
};


/* Com Config Container */
const ComConfig_type ComConfiguration =
{
 .ComTimeBase =
 {
  .ComRxTimeBase = 100,
  .ComTxTimeBase = 200
 },
 .ComIPdu = ComIPdu,
 .ComSignal = ComSignal,
 .ComSignalGroup=comsignalgroup,
 .ComGroupSignal=comgroupsignal
};





