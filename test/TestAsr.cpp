#include <gtest/gtest.h>
#include "asr_jd/asr.h"


TEST(TestAsr, should_implement_true)
{
    const char* domain = "search";
    const char* tocken = "./audio/ff274260-e101-11e8-af48-cb7cae820146.wav";
    const int ret = getTextFromAudio(domain, 1000, tocken);
    ASSERT_EQ(0, ret);
}
