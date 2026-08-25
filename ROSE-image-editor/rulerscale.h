#pragma once

// Zoom-adaptive ruler / grid spacing.
//
// `major` is the interval (in image pixels) between *labeled* ticks. It is
// chosen so that two consecutive labels stay at least ~60 screen pixels apart
// at the current zoom, which guarantees the numbers never overlap regardless
// of zoom level.
//
// `minor` is an unlabeled sub-interval that divides `major` evenly and stays
// at least ~8 screen pixels apart, giving the ruler/grid its finer look.
struct RulerScale
{
    int major = 100;   // image pixels between labeled ticks
    int minor = 20;    // image pixels between minor (unlabeled) ticks
};

// Compute a RulerScale for a given zoom factor. Zoomed out -> coarse steps
// (hundreds/thousands of pixels); zoomed in -> fine steps (units/tens).
RulerScale ComputeRulerScale(float zoom);
