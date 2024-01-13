#include "SimplePeakProcessor.hpp"
#include "gtest/gtest.h"

namespace jackmeter {
class SimplePeakProcessorTest : public ::testing::Test {
protected:
    std::unique_ptr<SimplePeakProcessor> processor;

    void SetUp() override
    {
        processor = std::make_unique<SimplePeakProcessor>("Test");
    }
};


TEST_F(SimplePeakProcessorTest, TestGetLatestPeak)
{
    float samples[] = { 0.0f, 0.5f, 1.0f };
    processor->Process(samples, 3);
    EXPECT_FLOAT_EQ(1.0f, processor->GetLatestPeak());
}


TEST_F(SimplePeakProcessorTest, TestGetMinPeak)
{
    float samples[] = { 0.0f, 0.5f, 1.0f };
    processor->Process(samples, 3);
    
    float samples2[] = { 0.0f, 0.5f, 0.0f };
    processor->Process(samples2, 3);

    EXPECT_FLOAT_EQ(0.5f, processor->GetMinPeak());
}

TEST_F(SimplePeakProcessorTest, TestGetMaxPeak)
{
    float samples[] = { 0.0f, 0.5f, 1.0f };
    processor->Process(samples, 3);
    EXPECT_FLOAT_EQ(1.0f, processor->GetMaxPeak());
}

} // namespace jackmeter