#include "config.h"
#include "Task_Parse.h"

#define NOTE_QUEUE_SIZE 10
#define NOTE_QUEUE_EMPTY (NoteQueueFront == NoteQueueRear)
#define NOTE_QUEUE_FULL ((NoteQueueRear + 1) % NOTE_QUEUE_SIZE == NoteQueueFront)

GlobalSettings_t GlobalSettings;
Note_Parse_t NoteQueue[NOTE_QUEUE_SIZE];
uint8_t NoteQueueFront = 0, NoteQueueRear = 0;

void ParseNote(Note_Token_t *token);
void QueueAddNote(Note_Parse_t *note);

void Task_Parse(void *parameters)
{
    GlobalSettings.Tonality = 0;
    GlobalSettings.Speed = 90.0f;
    Note_Token_t NoteToken;
    NoteToken.ScaleDegree = 0;
    NoteToken.DurationInBeats = 0;

    char tmString[] =
    "55_^6_|2-|11_^6,_|2-|"
    "55|6_^1'_6_5_|11_^6,_|2-|"
    "52|17,_^6,_|5,5|23_2_|11_^6,_|"
    "2_3_2_1_|2_^1_7,_^6,_|5,-^|5,0||";
    GlobalSettings.Tonality = 5; //1=F
    char *pchar = tmString;
    while(*pchar != '\0')
    {
        switch(*pchar)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            ParseNote(&NoteToken);
            NoteToken.ScaleDegree = *pchar - '0';
            NoteToken.DurationInBeats = 1;
            NoteToken.OctavesCount = 0;
            NoteToken.SemitonesCount = 0;
            NoteToken.DotCount = 0;
            NoteToken.hasTie = 0;
            break;
        case '\'':
            NoteToken.OctavesCount++;
            break;
        case ',':
            NoteToken.OctavesCount--;
            break;
        case '#':
            NoteToken.SemitonesCount++;
            break;
        case 'b':
            NoteToken.SemitonesCount--;
            break;
        case '-':
            NoteToken.DurationInBeats++;
            break;
        case '_':
            NoteToken.DurationInBeats /= 2;
            break;
        case '=':
            NoteToken.DurationInBeats /= 4;
            break;
        case '.':
            NoteToken.DotCount++;
            break;
        case '^':
            NoteToken.hasTie = 1;
            break;
        }
        pchar++;
        taskYIELD();
    }
    ParseNote(&NoteToken); //解析最后一个音符
		while(1)
    {
        taskYIELD();
    }
}

void ParseNote(Note_Token_t *token)
{
    Note_Parse_t temp;
    float duration = token->DurationInBeats * (2.0f - powf(2.0f, -token->DotCount)) * (60 / GlobalSettings.Speed);
    if(token->ScaleDegree == 0)
    {
        temp.Frequency = 0;
        temp.Duration = duration;
        QueueAddNote(&temp);
    }
    else
    {
        int8_t pitch;
        switch(token->ScaleDegree)
        {
        case 1:
            pitch = -9;
            break;
        case 2:
            pitch = -7;
            break;
        case 3:
            pitch = -5;
            break;
        case 4:
            pitch = -4;
            break;
        case 5:
            pitch = -2;
            break;
        case 6:
            pitch = 0;
            break;
        case 7:
            pitch = 2;
            break;
        }
        pitch += (token->OctavesCount * 12 + token->SemitonesCount + GlobalSettings.Tonality);
        if(token->hasTie)
        {
            temp.Frequency = 1760 * powf(2.0f, pitch / 12.0f); //蜂鸣器额定工作频率 2700Hz，默认升高两个八度
            temp.Duration = duration;
            QueueAddNote(&temp);
        }
        else
        {
            temp.Frequency = 1760 * powf(2.0f, pitch / 12.0f);
            temp.Duration = duration * 7 / 8;
            QueueAddNote(&temp);
            temp.Frequency = 0;
            temp.Duration = duration / 8;
            QueueAddNote(&temp);
        }
    }
}

void QueueAddNote(Note_Parse_t *note)
{
    while(NOTE_QUEUE_FULL)
    {
        taskYIELD();
    }
    NoteQueueRear = (NoteQueueRear + 1) % NOTE_QUEUE_SIZE;
    NoteQueue[NoteQueueRear] = *note;
}

Note_Parse_t* GetNextNote()
{
    if(NOTE_QUEUE_EMPTY)
    {
        return NULL;
    }
    NoteQueueFront = (NoteQueueFront + 1) % NOTE_QUEUE_SIZE;
    return &(NoteQueue[NoteQueueFront]);
}
