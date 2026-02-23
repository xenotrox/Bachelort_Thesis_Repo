#include "BandpassFilter6thOrder.h"

void BandpassFilter6thOrder::init(float sosCoeffs[][6]) {
  for (int i = 0; i < numSections; i++) {
    for (int j = 0; j < 6; j++) {
      sos[i][j] = sosCoeffs[i][j];
    }
    w[i][0] = 0.0f;
    w[i][1] = 0.0f;
  }
}

float BandpassFilter6thOrder::update(float input) {
  float output = input;
  for (int i = 0; i < numSections; i++) {
    float b0 = sos[i][0], b1 = sos[i][1], b2 = sos[i][2];
    float a0 = sos[i][3], a1 = sos[i][4], a2 = sos[i][5];

    if (a0 != 1.0f) { // Normalization [cite: 192]
      b0 /= a0; b1 /= a0; b2 /= a0;
      a1 /= a0; a2 /= a0;
    }

    // Direct Form II Transposed implementation [cite: 207]
    float w_new = output - a1 * w[i][0] - a2 * w[i][1];
    output = b0 * w_new + b1 * w[i][0] + b2 * w[i][1];

    w[i][1] = w[i][0];
    w[i][0] = w_new;
  }
  return output;
}