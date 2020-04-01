#include "Rte.h"
#include "Rtetypes.h"
#include "Rte_SeatHeatingController.h"


/*this function is called when notification action is notify
 * when signal invalidate is applied in receiver side for signal0*/
void com_notification(void)
{
    uint8 x = 30;
    x = x+30;
}


/*this function is called when notification action is notify
 * when signal invalidate is applied in receiver side for signal1*/
void com_notification_1(void)
{
    uint8 x = 30;
    x = x+30;
}

boolean seatsensorright =1;
boolean seatsensorleft =1;
uint32 Regulator =2 ,Regulator1=3, Regulator2=4;
uint8 OFF = 1;

void HeatingControllerRunnable(	)
	{
      //  Rte_Read_RightSeatStatus_PassengerOnRightSeat(&seatsensorright);
      // Rte_Read_LeftSeatStatus_PassengerOnLeftSeat(&seatsensorleft);
       //Rte_Read_RegulatorPosition_Position(&Regulator);

        if(seatsensorright == TRUE)
        {
            Rte_Write_RightLevel_Right_Level_Interface(&Regulator);
        }
        else if(seatsensorright == FALSE)
        {
            Rte_Write_RightLevel_Right_Level_Interface(&OFF);
        }
        if(seatsensorleft == TRUE)
        {
            Rte_Write_LeftLevel_Left_Level_Heater(&Regulator);
            Rte_Write_firstSignal(&Regulator);
            Rte_Write_secondSignal(&Regulator2);
            Rte_Write_thirdSignal(&Regulator1);
        }
        else if(seatsensorleft == FALSE)
        {
            Rte_Write_LeftLevel_Left_Level_Heater(&OFF);
        }
	}

