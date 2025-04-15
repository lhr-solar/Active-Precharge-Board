/* CAN test

- Setups up CANs in loopback mode
- Send 4 messages (since there are 3 mailboxes, 1 ends up going in the can1 send queue)
- receives the 4 messages and verifies correctness
- Flashes LED if successful on all CANs

*/

#include "stm32xx_hal.h"
#include "CAN.h"
#include "statusLeds.h"
#include "common.h"

StaticTask_t task_buffer;
StackType_t task_stack[configMINIMAL_STACK_SIZE];

static void error_handler(void) {
  while(1) {
    Status_Leds_Toggle(MOTOR_FAULT_LED);
    HAL_Delay(500);
  }
}

static void success_handler(void) {
  Status_Leds_Toggle(ARRAY_FAULT_LED);
  HAL_Delay(500);
}

static void task(void *pvParameters) {
  while(1){
    // create payload to send
    CAN_TxHeaderTypeDef tx_header = {0};   
    tx_header.StdId = 0x1;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.IDE = CAN_ID_STD;
    tx_header.DLC = 2;
    tx_header.TransmitGlobalTime = DISABLE;

    // send two payloads to 0x1
    uint8_t tx_data[8] = {0};
    tx_data[0] = 0x01;
    tx_data[1] = 0x00;
    if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

    tx_data[0] = 0x02;
    if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

    // send two payloads to 0x3
    tx_data[0] = 0x03;
    tx_header.StdId = 0x003;
    if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

    tx_data[0] = 0x04;
    if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

    // receive what was sent to 0x1
    CAN_RxHeaderTypeDef rx_header = {0};
    uint8_t rx_data[8] = {0};
    can_status_t status;

    // CAN1
    status = can_recv(hcan1, 0x1, &rx_header, rx_data, portMAX_DELAY);
    if (status != CAN_RECV && rx_data[0] != 0x1) error_handler();
    status = can_recv(hcan1, 0x1, &rx_header, rx_data, portMAX_DELAY);
    if (status != CAN_RECV && rx_data[0] != 0x2) error_handler();

    // make sure we don't receive from wrong ID and nonblocking works
    // CAN1
    status = can_recv(hcan1, 0x1, &rx_header, rx_data, 0);
    if (status != CAN_EMPTY) error_handler();
    status = can_recv(hcan1, 0x1, &rx_header, rx_data, 0);
    if (status != CAN_EMPTY) error_handler();

    // receive the rest
    // CAN1
    status = can_recv(hcan1, 0x3, &rx_header, rx_data, portMAX_DELAY);
    if (status != CAN_RECV && rx_data[0] != 0x3) error_handler();
    status = can_recv(hcan1, 0x3, &rx_header, rx_data, portMAX_DELAY);
    if (status != CAN_RECV && rx_data[0] != 0x4) error_handler();

    // TEST QUEUE OVERWRITE ============================================

    // send one payload to 0x4
    tx_data[0] = 0x04;
    tx_header.StdId = 0x004;
    if (can_send(hcan1, &tx_header, tx_data, true) != CAN_SENT) error_handler();

    // receive what was sent to 0x4
    status = can_recv(hcan1, 0x4, &rx_header, rx_data, true);
    if (status != CAN_RECV && rx_data[0] != 0x4) error_handler();
    
    // send two payloads to 0x4, only the last one should be received
    tx_data[0] = 0x05;
    if (can_send(hcan1, &tx_header, tx_data, true) != CAN_SENT) error_handler();
    tx_data[0] = 0x06;
    if (can_send(hcan1, &tx_header, tx_data, true) != CAN_SENT) error_handler();
    tx_data[0] = 0x07;
    if (can_send(hcan1, &tx_header, tx_data, true) != CAN_SENT) error_handler();
    tx_data[0] = 0x08;
    if (can_send(hcan1, &tx_header, tx_data, true) != CAN_SENT) error_handler();
    tx_data[0] = 0x09;
    if (can_send(hcan1, &tx_header, tx_data, true) != CAN_SENT) error_handler();

    HAL_Delay(200);

    // receive the rest in order
    status = can_recv(hcan1, 0x4, &rx_header, rx_data, true);
    if (status != CAN_RECV || rx_data[0] != 0x6) error_handler();
    status = can_recv(hcan1, 0x4, &rx_header, rx_data, true);
    if (status != CAN_RECV || rx_data[0] != 0x7) error_handler();
    status = can_recv(hcan1, 0x4, &rx_header, rx_data, true);
    if (status != CAN_RECV || rx_data[0] != 0x8) error_handler();
    status = can_recv(hcan1, 0x4, &rx_header, rx_data, true);
    if (status != CAN_RECV || rx_data[0] != 0x9) error_handler();

    success_handler();
  }
}

int main(void) {
  // initialize the HAL and system clock
  if (HAL_Init() != HAL_OK) error_handler();
  Status_Leds_Init();
  sys_clock_config();
  Status_Leds_Write(MOTOR_TIMEOUT_FAULT_LED, true);
  // create filter
  CAN_FilterTypeDef  sFilterConfig;
  can_filter_config(&sFilterConfig);

  // setup can1 init
  can1_config();

  // initialize CAN1
  if (can_init(hcan1, &sFilterConfig) != CAN_OK) error_handler();
  if (can_start(hcan1) != CAN_OK) error_handler();

  xTaskCreateStatic(
                task,
                "task",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                task_stack,
                &task_buffer);

  vTaskStartScheduler();

  error_handler();

  return 0;
}
