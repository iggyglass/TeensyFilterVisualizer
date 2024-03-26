#ifndef __MOVING_AVG_H
#define __MOVING_AVG_H

#include <cstdint>

template<std::uint8_t size=128>
class MovingAvg
{
private:
    float samples[size];
    int current;
    float sum;

public:
    MovingAvg()
    {
        current = 0;
        sum = 0;

        for (int i = 0; i < size; i++)
        {
            samples[i] = 0;
        }
    }

    void Sample(float value)
    {
        current = (current + 1) % size;

        sum -= samples[current];
        samples[current] = value;
        sum += value;
    }

    float Get()
    {
        return sum / size;
    }
};

#endif
