#include "args.h"

#include <stdlib.h>

#include <iostream>
#include <stdexcept>

namespace poincare {

Args::Args() {
    start_lr = 0.5;
    end_lr = 0.5;
    dimension = 10;
    checkpoint_interval = -1;
    distribution_power = 1;
    epochs = 5;
    number_negatives = 10;
    threads = 4;
    init_range = 1e-4;
    seed = 1;
}

void Args::parse_args(const std::vector<std::string>& args) {
    for (int ai = 1; ai < args.size(); ai += 2) {
        if (args[ai][0] != '-') {
            std::cerr << "Provided argument without a dash! Usage:" << std::endl;
            print_help();
            exit(EXIT_FAILURE);
        }
        try {
            if (args[ai] == "-h") {
                std::cerr << "Here is the help! Usage:" << std::endl;
                print_help();
                exit(EXIT_FAILURE);
            } else if (args[ai] == "-graph") {
                graph = std::string(args.at(ai + 1));
            } else if (args[ai] == "-input-vectors") {
                input_vectors = std::string(args.at(ai + 1));
            } else if (args[ai] == "-output-vectors") {
                output_vectors = std::string(args.at(ai + 1));
			} else if (args[ai] == "-start-lr") {
				start_lr = std::stof(args.at(ai + 1));
			} else if (args[ai] == "-end-lr") {
				end_lr = std::stof(args.at(ai + 1));
            } else if (args[ai] == "-distribution-power") {
                distribution_power = std::stof(args.at(ai + 1));
            } else if (args[ai] == "-init-range") {
                init_range = std::stof(args.at(ai + 1));
            } else if (args[ai] == "-dimension") {
                dimension = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-epochs") {
                epochs = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-checkpoint-interval") {
                checkpoint_interval = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-number-negatives") {
                number_negatives = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-threads") {
                threads = std::stoi(args.at(ai + 1));
            } else if (args[ai] == "-seed") {
                seed = std::stoi(args.at(ai + 1));
            } else {
                std::cerr << "Unknown argument: " << args[ai] << std::endl;
                print_help();
                exit(EXIT_FAILURE);
            }
        } catch (std::out_of_range) {
            std::cerr << args[ai] << " is missing an argument" << std::endl;
            print_help();
            exit(EXIT_FAILURE);
        }
    }
    if (graph.empty() || output_vectors.empty()) {
        std::cerr << "Empty graph or output-vectors path." << std::endl;
        print_help();
        exit(EXIT_FAILURE);
    }
}

void Args::print_help() {
    std::cerr
        << "    -graph                      training file path\n"
        << "    -output-vectors             file path for trained vectors\n"
        << "    -input-vectors              file path for init vectors (optional)\n"
        << "    -start-lr                   start learning rate [" << start_lr << "]\n"
        << "    -end-lr                     end learning rate [" << end_lr << "]\n"
        << "    -dimension                  manifold dimension [" << dimension << "]\n"
        << "    -init-range                 range of components for uniform initialization [" << init_range << "]\n"
        << "    -epochs                     number of epochs [" << epochs << "]\n"
        << "    -number-negatives           number of negatives sampled [" << number_negatives << "]\n"
        << "    -distribution-power         exponent to use to modify negative sampling distribution [" << distribution_power << "]\n"
        << "    -checkpoint-interval        save vectors every this many epochs [" << checkpoint_interval << "]\n"
        << "    -threads                    number of threads [" << threads << "]\n"
        << "    -seed                       seed for the random number generator [" << seed << "]\n"
        << "                                  n.b. only deterministic if single threaded!\n";
}
}
