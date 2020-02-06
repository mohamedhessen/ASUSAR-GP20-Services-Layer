
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
uint8 ComSignalBuffer_1 [2];
uint8 ComSignalBuffer_2 [1];
uint8 ComSignalBuffer_3 [4];
uint8 ComSignalBuffer_4 [2];
uint8 ComSignalBuffer_5 [1];
uint8 ComSignalBuffer_6 [4];

const ComSignal_type ComSignal[] = 
{
    {	//signal0
    	.ComBitPosition= 0,
        .ComUpdateBitPosition= 16 ,
        .ComHandleId= 0 ,
        .ComBitSize= 16,
        .ComSignalType = UINT16,
        .ComTransferProperty = TRIGGERED_ON_CHANGE,
        .ComIPduHandleId=0,
        .ComNotification=NULL,
		.ComSignalDataPtr = ComSignalBuffer_1
    },
    {	//signal1
        .ComBitPosition= 17,
        .ComUpdateBitPosition= 25,
        .ComHandleId= 1 ,
        .ComBitSize= 8,
        .ComSignalType = BOOLEAN,
        .ComTransferProperty = TRIGGERED_ON_CHANGE,
        .ComIPduHandleId=0,
		.ComNotification=NULL,
		.ComSignalDataPtr = ComSignalBuffer_2
    },
    {	//signal2
        .ComBitPosition= 26,
        .ComUpdateBitPosition= 58 ,
        .ComHandleId= 2 ,
        .ComBitSize= 32,
        .ComSignalType = UINT32,
        .ComTransferProperty = TRIGGERED_ON_CHANGE,
        .ComIPduHandleId=0,
		.ComNotification=NULL,
		.ComSignalDataPtr = ComSignalBuffer_3
    },
    {   //signal0
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
    {	//signal3
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
    {	//signal4
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
    },

};

/* IPdu signal lists. */
const ComSignal_type * const ComIPduSignalRefs_Can_Message_1[] = {
    &ComSignal[ CanDB_Signal_32_21_BE_Tester ],
    &ComSignal[ CanDB_Signal_1_4_LE_Tester ],
    &ComSignal[ CanDB_Signal_45_12_LE_Tester ],
	NULL
};

const ComSignal_type * const ComIPduSignalRefs_Can_Message_2[] = {
    &ComSignal[ CanDB_Signal_32_21_BE ],
    &ComSignal[ CanDB_Signal_29_12_BE_Tester],
    &ComSignal[ CanDB_Signal_1_4_LE ],
	NULL
};
const ComSignal_type * const ComIPduSignalRefs_Can_Message_3[] = {
    &ComSignal[ CanDB_Signal_1_4_LE ],
    NULL
};
/* IPdu buffers and signal */
    uint8 ComIPduBuffer_1[8]= {'M','o','h','a','m','e','d','F'};

uint8 ComIPduBuffer_2[8];

uint8 ComIPduBuffer_3[8];

/* Com Ipdu */
const ComIPdu_type ComIPdu[] = 
{
    { // CanDB_Message_1
        .ComIPduDirection = SEND ,
		.ComIPduSize=8,
        .ComIPduHandleId = 0 ,
        .ComIPduSignalRef =ComIPduSignalRefs_Can_Message_1,
		.ComIPduDataPtr=ComIPduBuffer_1,
		.ComIPduType = TP,
        .ComTxIPdu =
        {
             .ComTxModeFalse =
             {
                  .ComTxMode=
                  {
                    .ComTxModeMode = PERIODIC,
                    .ComTxModeNumberOfRepetitions = 2,
					.ComTxModeTimePeriod=1000,
					.ComTxModeRepetitionPeriod=15,
                  }
             },
						 
             .ComMinimumDelayTime = 0,
             .ComTxIPduClearUpdateBit = CONFIRMATION,
             .ComTxIPduUnusedAreasDefault = 255,
						 
        }
    },
    { // CanDB_Message_2
        .ComIPduDirection = RECEIVE ,
		.ComIPduSize=8,
		.ComIPduSignalProcessing = IMMEDIATE,
        .ComIPduHandleId = 1 ,
        .ComIPduSignalRef = ComIPduSignalRefs_Can_Message_2,
		.ComIPduDataPtr=ComIPduBuffer_2,
		.ComIPduType = TP,
        /*.ComTxIPdu =
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
        }*/
    },
    { // CanDB_Message_2
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
        }
};


/* Com Config Container */
const ComConfig_type ComConfiguration =
{
    .ComTimeBase =
    {
         .ComRxTimeBase = 500,
         .ComTxTimeBase = 500
    },
    .ComIPdu = ComIPdu,
    .ComSignal = ComSignal
};





