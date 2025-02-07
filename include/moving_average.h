//
// Created by jd on 11/26/2019.
//

#ifndef movingaverage_h
#define movingaverage_h

#include <Arduino.h>

/**
   Source: https://stackoverflow.com/a/10990893/1345237
*/
template <typename T, typename U, std::size_t N>
struct Moving_Average {
    T values[N] = {};
    U average = 0;

    void add(T value) {
        for (std::size_t i = 1; i < N; ++i) {
            values[i - 1] = values[i];
        }
        values[N - 1] = value;
        U sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            sum += values[i];
        }
        average = sum / N;
    }
};

#endif