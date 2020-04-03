#include "include/Std_Types.h"
#include "include/Com_Types.h"
#include "include/Com_helper.h"
#include "include/Com_Cfg.h"

boolean validateSignalID (Com_SignalIdType SignalId)
{
	if(SignalId<COM_NUM_OF_SIGNAL)
		return TRUE;
	return FALSE;
}
boolean validateGroupSignalIDv2 (Com_SignalGroupIdType SignalId)
{
    if(SignalId<COM_NUM_OF_SIGNAL)
        return TRUE;
    return FALSE;
}

boolean validateSignalGeneral(Com_SignalGeneral SignalId)
{
   if(SignalId<COM_NUM_OF_SIGNAL)
       return TRUE;
   return FALSE;
}

uint64 power(uint8 x,uint8 y)
{
	uint64 result = x;

	if (y == 0)
		return 1;

	for (; y>1 ; y--)
	{
		result = result * x;
	}
	return result;
}

uint8 Asu_Ceil(uint8 Input)
{
    uint8 Result = Input/8;

    if(Input % 8 > 0)
        Result = Result + 1;


    return Result;
}

//boolean compare_float(uint64 f1, uint64 f2)
//{
// float precision = 0.0001;
// if (((f1 - precision) < f2) &&
//     ((f1 + precision) > f2))
//  {
//   return TRUE;
//  }
// else if ((f1-precision)>f2 && (f1+precision)>f2){
//     return TRUE;
// }
// else
//  {
//   return FALSE;
//  }
//}
