#ifndef TYPE_H
#define TYPE_H

typedef int CoorType;
typedef int SpeedType;
typedef int TimeType;
typedef int IDType;
typedef int PeriodType;
enum RequestType
{
    REQUEST_UPDATE,
    REQUEST_RANGE_QUERY,
    REQUEST_KNN_QUERY
};

enum ReturnType
{
    RETURN_FAIL = 0,
    RETURN_SUCCESS = 1
};

enum SignalType
{
    SIGNAL_START,
    SIGNAL_STOP
};

#endif // TYPE_H
