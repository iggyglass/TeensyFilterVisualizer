#include <unity.h>
#include <MovingAvg.hpp>

// Set up tests
void setUp() {}

// Clean up tests
void tearDown() {}

// This test should only pass if it correctly averages when not enough samples have been added
void testPartialAverage()
{
    MovingAvg<4> avg = {};
    avg.Sample(1.0);
    avg.Sample(2.0);

    TEST_ASSERT_EQUAL_FLOAT(1.5, avg.Get());
}

// This test should work regardless of whether the algorithm is numerically stable
void testAverage()
{
    MovingAvg<4> avg = {};
    avg.Sample(0.1);
    avg.Sample(0.4);
    avg.Sample(0.7);
    avg.Sample(0.5);

    TEST_ASSERT_EQUAL_FLOAT(0.425, avg.Get());
}

// This test should only pass if the circular queue works
void testCircularQueue()
{
    MovingAvg<4> avg = {};
    avg.Sample(90);
    avg.Sample(1.0);
    avg.Sample(2.0);
    avg.Sample(3.0);
    avg.Sample(4.0);
    avg.Sample(5.0);

    TEST_ASSERT_EQUAL_FLOAT(3.5, avg.Get());
}

// This test should only work if Neumaier's fix to Kahan is correctly implemented 
void testNeumaierFix()
{
    MovingAvg<4> avg = {};
    avg.Sample(1.0);
    avg.Sample(10e25);
    avg.Sample(1.0);
    avg.Sample(-10e25);

    TEST_ASSERT_EQUAL_FLOAT(0.5, avg.Get());
}

// This test should *only* work if the algorithm implements Kahan summation
void testKahanSummation()
{
    MovingAvg<3> avg = {};
    avg.Sample(10e25);
    avg.Sample(3.0);
    avg.Sample(-10e25);

    TEST_ASSERT_EQUAL_FLOAT(1.0, avg.Get());
}

int main(int argc, char** argv)
{
    UNITY_BEGIN();

    RUN_TEST(testAverage);
    RUN_TEST(testPartialAverage);
    RUN_TEST(testCircularQueue);
    RUN_TEST(testKahanSummation);
    RUN_TEST(testNeumaierFix);

    UNITY_END();
}
