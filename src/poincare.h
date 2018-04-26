#pragma once

#include <random>
#include <fstream>
#include <memory>

#include "args.h"
#include "digraph.h"
#include "sampler.h"
#include "model.h"
#include "real.h"
#include "vector.h"

namespace poincare {

static const int32_t NEGATIVE_TABLE_SIZE = 100000000; // increased from the original

class Poincare {
 protected:
    std::shared_ptr<Args> args_;
    std::shared_ptr<Digraph> digraph;
    std::shared_ptr<Sampler> sampler;

    std::shared_ptr<std::vector<Vector>> vectors_;

    std::shared_ptr<Model> model_;

    void save_checkpoint(int32_t epochs_trained);

 public:
    Poincare(std::shared_ptr<Args> args);

    void save_vectors(std::string);
    void load_vectors(std::string);
    void print_info(clock_t, real, int64_t, real, real);

    void epoch_thread(int32_t thread_id, uint32_t seed, real start_lr, real end_lr);
    void train();

};

}
