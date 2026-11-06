#include "SBusTracker.h"


SBusTracker::SBusTracker(int pre_load) {
    rolling_sum = 0;
    head_index = 0;
    for(int i = 0; i < HISTORY_SIZE; i++) {
        rolling_sum += pre_load;
        tracker_array[i] = pre_load;
    }
}

void SBusTracker::add(int instance) {
    rolling_sum -= tracker_array[head_index];
    rolling_sum += instance;
    tracker_array[head_index] = instance;
    head_index = (head_index + 1) % HISTORY_SIZE;
}

unsigned int SBusTracker::get_head_index() {
    return head_index;
}

unsigned int SBusTracker::get_rolling_sum() {
    return rolling_sum;
}

unsigned int SBusTracker::get_estimated() {
    return rolling_sum / HISTORY_SIZE;
}

void SBusTracker::print_arr(Serial_ & printer) {
    printer.print(head_index); 
    printer.print(" : {");
    for (int i = 0; i < HISTORY_SIZE; i++) {
        printer.print(tracker_array[i]);
        printer.print(", ");
    }
    printer.println("}");
}

