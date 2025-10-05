/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {
    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;
    int context_save_restore_time = 30; 


    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/

        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU Burst\n";
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL") {
            // duration_intr is the syscall number
            auto [boilerplate, updated_time] = intr_boilerplate(current_time, duration_intr, context_save_restore_time, vectors);
            execution += boilerplate;
            current_time = updated_time;

            // ISR step
            int ISR_activity_time = 40; 
            execution += std::to_string(current_time) + "," + std::to_string(ISR_activity_time) + ", SYSCALL: run the ISR (device driver)\n";
            current_time += ISR_activity_time; 

            execution += std::to_string(current_time) + "," + std::to_string(ISR_activity_time) + ", transfer data from device to memory\n";
            current_time += ISR_activity_time; 

            // The duration of the interrupt is given by the device table.
            // As such remaining time is used to check for errors
            int remaining_interrupt_time = delays[duration_intr] - 2 * ISR_activity_time;
            if (remaining_interrupt_time > 0) {
                execution += std::to_string(current_time) + "," + std::to_string(remaining_interrupt_time) + ", checked for errors\n";
                current_time += remaining_interrupt_time;
            }

            execution += std::to_string(current_time) + ", 1, IRET\n";
            current_time += 1;
        } 
        else if (activity == "END_IO") {
            // duration_intr is the syscall number
            auto [boilerplate, updated_time] = intr_boilerplate(current_time, duration_intr, context_save_restore_time, vectors);
            execution += boilerplate;
            current_time = updated_time;

            // ISR step
            int ISR_activity_time = 40; 
            execution += std::to_string(current_time) + "," + std::to_string(ISR_activity_time) + ", ENDIO: run the ISR (device driver)\n";
            current_time += ISR_activity_time; 

            // The duration of the interrupt is given by the device table.
            // As such remaining time is used to check device status
            int remaining_interrupt_time = delays[duration_intr] - ISR_activity_time;
            if (remaining_interrupt_time > 0) {
                execution += std::to_string(current_time) + "," + std::to_string(remaining_interrupt_time) + ", check device status\n";
                current_time += remaining_interrupt_time;
            }

            execution += std::to_string(current_time) + ", 1, IRET\n";
            current_time += 1;
        }

        /************************************************************************/

    }

    input_file.close();
    write_output(execution);

    return 0;
}
