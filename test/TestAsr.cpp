#include <gtest/gtest.h>
#include "asr_jd/asr.h"


TEST(TestAsr, should_return_success)
{
    const char* domain = "search";
    const char* tocken = "../test/audios/5cfe1890-ef18-11e8-b632-953d52ab294b.wav";
    const int ret = getTextFromAudio(domain, 10000, tocken);
    ASSERT_EQ(0, ret);
}
