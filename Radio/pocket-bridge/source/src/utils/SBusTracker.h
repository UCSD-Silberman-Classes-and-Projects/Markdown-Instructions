// SBusTracker.h
#include <Arduino.h>

#define HISTORY_SIZE 9

class SBusTracker {
    private:
        int tracker_array[HISTORY_SIZE];
        unsigned int head_index;
        unsigned int rolling_sum;

    public:
        SBusTracker(int pre_load);

        SBusTracker() : SBusTracker(0) {};

        void add(int instance);

        unsigned int get_head_index();

        unsigned int get_rolling_sum();

        unsigned int get_estimated();

        void print_arr(Serial_ & prtinter);
};