#include "common.h"

/**
* @brief Error handler that can be overriden by the user
* @retval None
*/
__weak void error_handler(){
  while(1){}
}

/**
* @brief System Clock Configuration
* @retval None
*/
void sys_clock_config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    error_handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    error_handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    error_handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
* @brief Configure the can filter
* @retval None
*/
void can_filter_config(CAN_FilterTypeDef* sFilterConfig){
  sFilterConfig->FilterBank = 0;
  sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig->FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig->FilterIdHigh = 0x0000;
  sFilterConfig->FilterIdLow = 0x0000;
  sFilterConfig->FilterMaskIdHigh = 0x0000;
  sFilterConfig->FilterMaskIdLow = 0x0000;
  sFilterConfig->FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig->FilterActivation = ENABLE;
  sFilterConfig->SlaveStartFilterBank = 14;
}

/**
* @brief Configure can1
* @retval None
*/
void can1_config(void){
  hcan1->Init.Prescaler = 40;

  #ifdef CAN_LOOPBACK
  hcan1->Init.Mode = CAN_MODE_LOOPBACK;
  #else
  hcan1->Init.Mode = CAN_MODE_NORMAL;
  #endif

  hcan1->Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1->Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1->Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1->Init.TimeTriggeredMode = DISABLE;
  hcan1->Init.AutoBusOff = DISABLE;
  hcan1->Init.AutoWakeUp = DISABLE;
  hcan1->Init.AutoRetransmission = ENABLE;
  hcan1->Init.ReceiveFifoLocked = DISABLE;
  
  // If TransmitFifoPriority is disabled, the hardware selects the mailbox based on the message ID priority. 
  // If enabled, the hardware uses a FIFO mechanism to select the mailbox based on the order of transmission requests.
  hcan1->Init.TransmitFifoPriority = ENABLE;
}