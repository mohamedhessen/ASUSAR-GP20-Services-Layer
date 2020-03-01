#include "Rte.h"
#include "Rtetypes.h"
#include "Rte_SeatHeatingController.h"


boolean seatsensorright =1;
boolean seatsensorleft =1;
uint32 Regulator =0x3f3f;
uint8 OFF = 1;
void HeatingControllerRunnable(	)
	{
      //  Rte_Read_RightSeatStatus_PassengerOnRightSeat(&seatsensorright);
      // Rte_Read_LeftSeatStatus_PassengerOnLeftSeat(&seatsensorleft);
      // Rte_Read_RegulatorPosition_Position(&Regulator);

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
        }
        else if(seatsensorleft == FALSE)
        {
            Rte_Write_LeftLevel_Left_Level_Heater(&OFF);
        }
	}

