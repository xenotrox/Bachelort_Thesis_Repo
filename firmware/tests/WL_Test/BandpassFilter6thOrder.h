#ifndef BANDPASSFILTER6THORDER_H
#define BANDPASSFILTER6THORDER_H

#include "Arduino.h"

class BandpassFilter6thOrder {
  public:
    void init(float sosCoeffs[][6]);
    float update(float input);

  private:
    static const int numSections = 3;   // 6th-order = 3 second-order sections 
    float sos[numSections][6];          // Filter coefficients
    float w[numSections][2];            // Delay elements (Direct Form II)
};

#endif