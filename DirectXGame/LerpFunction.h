#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

float Lerp(float a, float b, float t) { return a + t * (b - a); }

// １個目
float easeInSine(float t) { return 1.0f - cosf(t * (float)M_PI / 2.0f); }

// ２個目
float easeOutSine(float t) { return sinf((t * (float)M_PI) / 2.0f); }

// ３個目
float easeInOutSine(float t) { return -(cosf((float)M_PI * t) - 1.0f) / 2.0f; }

// ４個目
float easeInQuad(float t) { return t * t; }

// ５個目
float easeOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }

// ６個目
float easeInOutQuad(float t) { return t < 0.5f ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f; }

// ７個目
float easeInCubic(float t) { return t * t * t; }

// ８個目
float easeOutCubic(float t) { return 1.0f - powf(1.0f - t, 3.0f); }

// ９個目 案
float easeInOutCubic(float t) { return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f; }

// １０個目
float easeInQuart(float t) { return t * t * t * t; }

// １１個目　案
float easeOutQuart(float t) { return 1 - powf(1.0f - t, 4.0f); }

// １２個目　案
float easeInOutQuart(float t) { return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 4.0f) / 2.0f; }

// １３個目
float easeInQuint(float t) { return t * t * t * t * t; }

// １４個目 案
float easeOutQuint(float t) { return 1 - powf(1.0f - t, 5.0f); }

// １５個目
float easeInOutQuint(float t) { return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 5.0f) / 2.0f; }

// １６個目
float easeInExpo(float t) { return t == 0.0f ? 0.0f : powf(2.0f, 10.0f * t - 10.0f); }

// １７個目
float easeOutExpo(float t) { return t == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * t); }

// １８個目
float easeInOutExpo(float t) { return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f; }

// １９個目
float easeInCirc(float t) { return 1.0f - sqrtf(1.0f - powf(t, 2.0f)); }

// ２０個目　案
float easeOutCirc(float t) { return sqrtf(1.0f - powf(t - 1.0f, 2.0f)); }

// ２１個目 案
float easeInOutCirc(float t) { return t < 0.5f ? (1 - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f : (sqrtf(1 - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f; }

// ２２個目　案
float easeInBack(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return c3 * t * t * t - c1 * t * t;
}

// ２３個目 案
float easeOutBack(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
}

// ２４個目
float easeInOutBack(float t) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return t < 0.5f ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

// ２５個目
float easeInElastic(float t) {
	const float c4 = (2 * (float)M_PI) / 3.0f;

	return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
}

// ２６個目 案
float easeOutElastic(float t) {
	const float c4 = (2.0f * (float)M_PI) / 3.0f;

	return t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : powf(2.0, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
}

// ２７個目
float easeInOutElastic(float t) {
	const float c5 = (2 * (float)M_PI) / 4.5f;

	return t == 0.0f   ? 0.0f
	       : t == 1.0f ? 1
	       : t < 0.5f  ? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f
	                   : (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
}

// ２９個目 案
float easeOutBounce(float t) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1 / d1) {
		return n1 * t * t;
	} else if (t < 2 / d1) {
		return n1 * (t -= 1.5f / d1) * t + 0.75f;
	} else if (t < 2.5 / d1) {
		return n1 * (t -= 2.25f / d1) * t + 0.9375f;
	} else {
		return n1 * (t -= 2.625f / d1) * t + 0.984375f;
	}
}

// ２８個目
float easeInBounce(float t) { return 1 - easeOutBounce(1 - t); }

// ３０個目
float easeInOutBounce(float t) { return t < 0.5f ? (1.0f - easeOutBounce(1.0f - 2.0f * t)) / 2.0f : (1.0f + easeOutBounce(2.0f * t - 1.0f)) / 2.0f; }
