#include "config.h"
#include "Task_AudioPlay.h"
#include "Task_Parse.h"

#define TIM_FREQUENCY 1000000
#define TIM_HANDLE htim3
#define TIM_CHANNEL TIM_CHANNEL_1

void SetFrequency(float f);

void Task_AudioPlay(void *parameters)
{
    TickType_t xLastWakeUpTime;
    xLastWakeUpTime = xTaskGetTickCount();

    HAL_TIM_PWM_Start(&TIM_HANDLE, TIM_CHANNEL);
    Note_Parse_t *pnote;
    while(1)
    {
        pnote = GetNextNote();
        if(pnote == NULL)
        {
            SetFrequency(0);
            taskYIELD();
        }
        else
        {
            SetFrequency(pnote->Frequency);
            if (pnote->Duration > 0)
            {
                vTaskDelayUntil(&xLastWakeUpTime, pdMS_TO_TICKS(pnote->Duration * 1000));
            }
        }
    }
}

void SetFrequency(float f)
{
    if(f == 0)
    {
        __HAL_TIM_SET_AUTORELOAD(&TIM_HANDLE, 0);
        __HAL_TIM_SET_COMPARE(&TIM_HANDLE, TIM_CHANNEL, 0);
    }
    else
    {
        __HAL_TIM_SET_AUTORELOAD(&TIM_HANDLE, (uint32_t)(TIM_FREQUENCY / f));
        __HAL_TIM_SET_COMPARE(&TIM_HANDLE, TIM_CHANNEL, (uint32_t)(TIM_FREQUENCY / f / 2));
    }
}
