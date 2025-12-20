#ifndef ANCLOGIC_H
#define ANCLOGIC_H

void updatewk(float* wk,int16_t* x, float mu, const unsigned short int M, float en, unsigned short int n);
float calculate_y(float* wk, int16_t* x, const unsigned short int M, unsigned short int n);

#endif 