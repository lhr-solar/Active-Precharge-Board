#include "Task_Contactor.h"

void Task_Contactors(void* pvParamters) {
  contactors_init();
  contactors_handler();

  // TODO: idk if we need more error checking or smth here?
}