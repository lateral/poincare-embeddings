#include "poincare.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <algorithm>

// how many edges to process before reporting on performance
constexpr int32_t REPORTING_INTERVAL = 50;

namespace poincare {

Poincare::Poincare(std::shared_ptr<Args> args) {
    args_ = args;
}

void Poincare::save_vectors(std::string fn) {
    std::ofstream ofs(fn);
    if (!ofs.is_open()) {
        throw std::invalid_argument(fn + " cannot be opened!");
    }
    for (int32_t i = 0; i < digraph->node_count(); i++) {
        std::string name = (digraph->enumeration2node[i])->name;
        ofs << name << " " << vectors_->at(i) << std::endl;
    }
    ofs.close();
}

void Poincare::load_vectors(std::string fn) {
    std::ifstream in(fn);
    if (!in.is_open()) {
        throw std::invalid_argument(fn + " cannot be opened!");
    }
    std::string line;
    while (std::getline(in, line)) {
        std::string field;
        std::stringstream line_stream(line);
        // count the fields
        int col = 0;
        int32_t offset;
        while (std::getline(line_stream, field, ' ')) {
            if (col == 0) {
                offset = (digraph->name2node).at(field)->enumeration;
            } else {
                vectors_->at(offset)[col - 1] = std::stold(field);
            }
            col++;
        }
    }
    in.close();
}

void Poincare::save_checkpoint(int32_t epochs_trained) {
    if (args_->checkpoint_interval > 0 && epochs_trained % args_->checkpoint_interval == 0) {
        // checkpoint (save) the vectors - pad epoch number to maintain
        // alphabetical ordering
        std::string epochs_done = std::to_string(epochs_trained);
        epochs_done = std::string(6 - epochs_done.length(), '0') + epochs_done;
        this->save_vectors(args_->output_vectors + "-after-" + epochs_done + "-epochs");
    }
}

void Poincare::print_info(clock_t start, real progress, int64_t edges_processed, real lr, real performance) {
    real cpu_time_single_thread = real(clock() - start) / (CLOCKS_PER_SEC * args_->threads);
    real est = real(edges_processed) / cpu_time_single_thread;
    std::cerr << std::fixed;
    std::cerr << "\rProgress: " << std::setw(5) << std::setprecision(1) << 100 * progress << "%";
    std::cerr << "    edges/thread/s: " << std::setw(8) << std::setprecision(0) << est;
    std::cerr << "    lr: " << std::setw(5) << std::setprecision(3) << lr;
    std::cerr << "    objective: " << std::setw(5) << std::setprecision(3) << performance;
    std::cerr << std::flush;
}

void Poincare::train() {
    std::ifstream ifs(args_->graph);
    if (!ifs.is_open()) {
        throw std::invalid_argument(args_->graph + " cannot be opened!");
    }
    digraph = std::make_shared<Digraph>(ifs);
    ifs.close();
    
    // setup the negative sampler
    std::vector<int64_t> counts(digraph->node_count());
    for (int i=0; i < digraph->node_count(); i++) {
        counts[i] = (digraph->enumeration2node)[i]->count_as_target;
    }
    std::cerr << "Generating negative samples...\n";
    sampler = std::make_shared<Sampler>(args_->distribution_power, counts, NEGATIVE_TABLE_SIZE);
    // initialise the vectors
    std::minstd_rand rng(args_->seed);
    Vector init_vector(args_->dimension);
    vectors_ = std::make_shared<std::vector<Vector>>();
    for (int64_t i=0; i < digraph->node_count(); i++) {
        random_uniform_components(init_vector, rng, args_->init_range);
        vectors_->push_back(init_vector);
    }
    // overwrite the init vectors with any pre-trained vectors
    if (!(args_->input_vectors).empty()) {
        std::cerr << "Loading vectors: " << args_->input_vectors << "\n";
        load_vectors(args_->input_vectors);
    }
    // start the training!
    real lr_delta_per_epoch = (args_->start_lr - args_->end_lr) / args_->epochs;;
    for (int32_t epoch = 0; epoch < args_->epochs; epoch++) {
        save_checkpoint(epoch);
        std::cerr << "\rEpoch: " << (epoch + 1) << " / " << args_->epochs << "\n";
        std::cerr << std::flush;
        real epoch_start_lr = args_->start_lr - real(epoch) * lr_delta_per_epoch;
        real epoch_end_lr = args_->start_lr - real(epoch + 1) * lr_delta_per_epoch;
        std::vector<std::thread> threads;
        for (int32_t thread_id = 0; thread_id < args_->threads; thread_id++) {
            int32_t thread_seed = args_->seed + epoch * args_->threads + thread_id;
            threads.push_back(std::thread([=]() {
                epoch_thread(thread_id, thread_seed, epoch_start_lr, epoch_end_lr);
            }));
        }
        for (auto it = threads.begin(); it != threads.end(); ++it) {
            it->join();
        }
    }
    save_checkpoint(args_->epochs);
}

void Poincare::epoch_thread(int32_t thread_id, uint32_t seed, real start_lr, real end_lr) {
    std::minstd_rand rng(1 + seed); // seed 0 and 1 coincide for minstd_rand
    const int64_t edges_per_thread = digraph->edges.size() / args_->threads;
    Model model(vectors_, args_);

    int64_t iter_count = 0; // number processed so far
    clock_t start = clock();
    real lr = start_lr;
    real progress = 0.;
    std::vector<int32_t> samples;
    Edge* edge;
    for (int64_t i = thread_id; i < digraph->edges.size(); i=i+args_->threads) {
        edge = (digraph->edges)[i];
        iter_count++;
        int32_t source_enum = (edge->source).enumeration;
        int32_t target_enum = (edge->target).enumeration;
        progress = real(iter_count) / edges_per_thread;
        lr = start_lr * (1.0 - progress) + end_lr * progress;

        samples.clear();
        // first sample is the positive sample
        samples.push_back(target_enum);
        // draw some distinct negative samples, excluding positive samples
        while (samples.size() < args_->number_negatives + 1) {
            auto next_negative = sampler->get_sample(edge->source.target_enums, rng);
			if (std::find(samples.begin(), samples.end(), next_negative) != samples.end()) {
				continue; // already have this sample
            }
            samples.push_back(next_negative);
        }

        model.nickel_kiela_objective(source_enum, samples, lr);
        if (thread_id == 0) {
            // only thread 0 is responsible for printing progress info
            if (iter_count % REPORTING_INTERVAL == 0) {
                print_info(start, progress, iter_count, lr, model.get_performance());
            }
        }
    }
    if (thread_id == 0) {
        print_info(start, progress, iter_count, lr, model.get_performance());
        std::cerr << std::endl;
    }
}

}
