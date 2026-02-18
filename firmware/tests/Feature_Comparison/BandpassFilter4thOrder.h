#ifndef BANDPASSFILTER4THORDER_H
#define BANDPASSFILTER4THORDER_H

#include "Arduino.h"

class BandpassFilter4thOrder {
  public:
    void init(float sosCoeffs[][6]);
    float update(float input);

  private:
    static const int numSections = 2;   // Two second-order sections
    float sos[numSections][6];          // Filter coefficients
    float w[numSections][2];            // Delay elements for each section
};

#endif
