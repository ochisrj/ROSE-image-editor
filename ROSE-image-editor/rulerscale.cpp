#include "rulerscale.h"

RulerScale ComputeRulerScale(float zoom)
{
    // Minimum screen gap between two labeled ticks. Must be large enough for
    // the widest label (up to ~4 characters with the k/M shorthand, ~40px at
    // 16px) plus comfortable breathing room, so consecutive numbers can never
    // touch each other.
    constexpr float kLabelGapPx = 80.0f;
    // Minimum screen gap between the finer, unlabeled minor ticks.
    constexpr float kMinorGapPx = 8.0f;

    // Nice "human" step factors. All entries divide the next one cleanly so a
    // minor interval can always be derived as a clean fifth/half of major.
    static const int kFactors[] =
    {
        1, 2, 5, 10, 20, 25, 50, 100, 200, 250, 500,
        1000, 2000, 2500, 5000, 10000, 20000, 50000,
        100000, 200000, 500000, 1000000,
    };

    // Smallest factor whose on-screen spacing clears the label gap: zoomed in
    // this lands on units/tens (1, 2, 5, 10, 20...), zoomed out on hundreds
    // and thousands (100, 500, 1000...).
    int major = kFactors[0];
    for (int f : kFactors)
    {
        major = f;
        if ((float)f * zoom >= kLabelGapPx)
            break;
    }

    // Minor interval: start with a clean fifth of major, fall back to a half,
    // then to major itself if the zoom is too small to support finer ticks.
    int minor = major / 5;
    if (minor < 1)
        minor = 1;
    if ((float)minor * zoom < kMinorGapPx)
        minor = major / 2;
    if (minor < 1)
        minor = 1;
    if ((float)minor * zoom < kMinorGapPx)
        minor = major;

    return { major, minor };
}
