#include "config.h"
#include "Task_AudioPlay.h"
#include "Task_Parse.h"

TaskHandle_t TaskHandle_AudioPlay, TaskHandle_Parse;

void Task_Init(void *parameters)
{
    taskENTER_CRITICAL();

    xTaskCreate(Task_AudioPlay, "TaskAudioPlay", 200, NULL, 0, &TaskHandle_AudioPlay);
    xTaskCreate(Task_Parse, "TaskParse", 200, NULL, 0, &TaskHandle_Parse);

    vTaskDelete(NULL);
    taskEXIT_CRITICAL();
}
