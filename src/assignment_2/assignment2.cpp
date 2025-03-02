/*
// File: top.cpp
//
*/

#include <iostream>
#include <systemc.h>

#include "CPU.h"
#include "Cache.h"
#include "Bus.h"
#include "psa.h"

using namespace std;

int sc_main(int argc, char *argv[]) {
    try {
        // Get the tracefile argument and create Tracefile object
        // This function sets tracefile_ptr and num_cpus
        init_tracefile(&argc, &argv);

        // init_tracefile changed argc and argv so we cannot use
        // getopt anymore.
        // The "-q" flag must be specified _after_ the tracefile.
        if (argc == 2 && !strcmp(argv[0], "-q")) {
            sc_report_handler::set_verbosity_level(SC_LOW);
        }

        sc_set_time_resolution(1, SC_PS);

        // Initialize statistics counters
        stats_init();

        // Create instances with id 0
        CPU *cpu = new CPU(sc_gen_unique_name("cpu"), 0);
        Cache *cache = new Cache(sc_gen_unique_name("cache"), 0);
        Memory *memory = new Memory("memory");
        Bus *bus = new Bus("bus");

        // The clock that will drive the CPU
        sc_clock clk;

        // Connect instances
        cpu->cache(*cache);
        //cache->memory(*memory);
        cache->bus(*bus);
        bus->memory(*memory);

        cpu->clock(clk);
        cache->clk(clk);
        bus->clk(clk);

        bus->add_cache(cache);

        // Start Simulation
        sc_start();

        // Print statistics after simulation finished
        stats_print();
        cout << sc_time_stamp() << endl;

        // Cleanup components
        delete cpu;
        delete cache;
        delete memory;
        delete bus;
    } catch (exception &e) {
        cerr << e.what() << endl;
    }

    return 0;
}
