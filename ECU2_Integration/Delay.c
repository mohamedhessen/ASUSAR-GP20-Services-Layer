#pragma CHECK_MISRA("none")
#include <stdint.h>


//Function to delay t microseconds
/*
void Delay_Us(long t){
	  long i , j;
	   i = 0;
	   j = 0;
	  while(i<t){
       while(j<3){
				 j++;
			 //Do Nothing
			 }
			 i++;
	  }
}
//Function to delay t milliseconds
void Delay_ms(long t){
	  long i , j;
	   i = 0;
	   j = 0;
	  while(i<t){
       while(j<3180){
				 j++;
			 //Do Nothing
			 }
			 i++;
	  }
}
*/void Delay_Us(int32_t t){
      int32_t i , j;
       i = 0;
       j = 0;
      while(i<t){
       while(j<3){
                 j++;
             //Do Nothing
             }
             i++;
      }
}
//Function to delay t milliseconds
void Delay_ms(int32_t t){
      int32_t i , j;
       i = 0;
       j = 0;
      while(i<t){
       while(j<3180){
                 j++;
             //Do Nothing
             }
             i++;
      }
}
