#include "sampler.h"
#include <algorithm>

namespace poincare {

    Sampler::Sampler(real distribution_power_, const std::vector<int64_t>& counts, int64_t table_size) : distribution_power (distribution_power_) {
        real z = 0.0;
        for (size_t i = 0; i < counts.size(); i++) {
            z += pow(counts[i], distribution_power);
        }
        for (size_t i = 0; i < counts.size(); i++) {
            real c = pow(counts[i], distribution_power);
            for (size_t j = 0; j < c * table_size / z; j++) {
                samples.push_back(i);
            }
        }
    }

    int32_t Sampler::get_sample(std::vector<int32_t> exclude, std::minstd_rand& rng) const {
        int32_t sample;
        do {
            sample = samples[rng() % samples.size()];
        } while (std::find(exclude.begin(), exclude.end(), sample) != exclude.end());
        return sample;
    }
}
