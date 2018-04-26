#pragma once

#include <random>

#include "real.h"

namespace poincare {

class Sampler {
    protected:
        std::vector<int32_t> samples;
        const real distribution_power;

    public:
        /**
         * `distribution_power_` is the power to which the counts are raised
         * prior to normalisation.
         * `counts` gives observed number of occurrences of each outcome.
         **/
        Sampler(real distribution_power_, const std::vector<int64_t>& counts, int64_t table_size);

        /**
         * Draw a single sample. 
         */
        int32_t get_sample(std::vector<int32_t> exclude, std::minstd_rand& rng) const;
};
}
