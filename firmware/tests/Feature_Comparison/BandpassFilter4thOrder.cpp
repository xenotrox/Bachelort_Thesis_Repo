#include "BandpassFilter4thOrder.h"

void BandpassFilter4thOrder::init(float sosCoeffs[][6]) {
  // Copy the coefficients into the class member
  for (int i = 0; i < numSections; i++) {
    for (int j = 0; j < 6; j++) {
      sos[i][j] = sosCoeffs[i][j];
    }
    // Initialize delay elements to zero
    w[i][0] = 0.0f;
    w[i][1] = 0.0f;
  }
}

float BandpassFilter4thOrder::update(float input) {
  float output = input;
  for (int i = 0; i < numSections; i++) {
    float b0 = sos[i][0];
    float b1 = sos[i][1];
    float b2 = sos[i][2];
    float a0 = sos[i][3];
    float a1 = sos[i][4];
    float a2 = sos[i][5];

    // Normalize coefficients if a0 is not 1
    if (a0 != 1.0f) {
      b0 /= a0;
      b1 /= a0;
      b2 /= a0;
      a1 /= a0;
      a2 /= a0;
    }

    // Direct Form II Transposed implementation
    float w_new = output - a1 * w[i][0] - a2 * w[i][1];
    output = b0 * w_new + b1 * w[i][0] + b2 * w[i][1];

    // Update delay elements
    w[i][1] = w[i][0];
    w[i][0] = w_new;
  }
  return output;
}
