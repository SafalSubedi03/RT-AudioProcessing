#include<iostream>
#include<cmath>
using namespace std;

// use for learning purposes 
/*
    y[n] = wk*u[n-k]
    d[n] = u[n] + v[n]
    e[n] = d[n] - y[n]

    w[n+1] = w[n] + (mu/input_power) * e[n] * u[n-k]

*/
    


void updatewk(float* wk,int16_t* x, float mu, const unsigned short int M, float en, unsigned short int n){
    float input_power = 1e-6f;
    for(int i = 0; i < M ; i++){
        input_power += x[n-i]*x[n-i];
    }

    for(int i = 0; i < M; i++){
        wk[i] = wk[i] + (mu / input_power) * en * x[n-i];
    }
}

float calculate_y(float* wk, int16_t* x, const unsigned short int M, unsigned short int n){
    float y = 0.0f;
    for(int i=0;i<M;i++){
        y += wk[i] * x[n-i];
    }
    return y;
}