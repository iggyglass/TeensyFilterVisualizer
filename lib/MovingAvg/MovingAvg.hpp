#ifndef __MOVING_AVG_H
#define __MOVING_AVG_H

#include <cstdint>
#include <cmath>

template<std::uint8_t size=128>
class MovingAvg
{
private:
    float samples[size] = {0};
    int current = 0;
    int count = 0;

    float sum = 0;
    float correction = 0;

private:
    void kahanBabushkaAdd(float value)
    {
        float t = sum + value;

        correction += std::abs(sum) >= std::abs(value) ? (sum - t) + value : (value - t) + sum;
        sum = t;
    }

public:
    void Sample(float value)
    {
        if (count < size) count++;

        current = (current + 1) % size;

        kahanBabushkaAdd(-samples[current]);
        kahanBabushkaAdd(value);
        samples[current] = value;
    }

    float Get()
    {
        return (sum + correction) / count;
    }
};

#endif
