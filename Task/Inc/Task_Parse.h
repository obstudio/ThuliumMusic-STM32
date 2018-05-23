typedef struct
{
    int8_t Tonality;
    float Speed;
} GlobalSettings_t;

typedef struct
{
    int8_t ScaleDegree, SemitonesCount, OctavesCount, DotCount, hasTie;
    float DurationInBeats;
} Note_Token_t;

typedef struct
{
    float Frequency, Duration;
} Note_Parse_t;

void Task_Parse(void *parameters);
Note_Parse_t* GetNextNote(void);
