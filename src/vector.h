#pragma once

#include <cstdint>
#include <ostream>
#include <random>
#include <assert.h>

#include "real.h"

namespace poincare {

class Vector {

    public:
        int64_t dimension_;
        real* data_;

        explicit Vector(int64_t);
        explicit Vector(const Vector&);
        ~Vector();

        Vector& operator= (const Vector&);
        real& operator[](int64_t);
        const real& operator[](int64_t) const;

        /**
         * Return the length of this vector.
         */
        int64_t size() const;

        /**
         * Set all entries to zero.
         */
        void zero();

        /**
         * Multiply all entries by the given value, in place.
         */
        void multiply(real);

        /**
         * Add the given vector to this vector.
         */
        void add(const Vector& source);

        /**
         * Add the specified multiple of the given vector to this vector.
         */
        void add(const Vector& other_vector, real scalar);

		real squared_norm() const;
};

std::ostream& operator<<(std::ostream&, const Vector&);

/*
 * Return the inner product of the two vectors provided.
 */
inline real dot(const Vector& v, const Vector& w) {
    real result = 0;
    for (int64_t i = 0; i < v.size(); ++i) {
        result += v[i]*w[i];
    }
    return result;
}

inline real squared_dist(const Vector& v, const Vector& w) {
    real result = 0;
    for (int64_t i = 0; i < v.size(); ++i) {
        result += pow(v[i] - w[i], 2);
    }
    return result;
}

void random_uniform_components(Vector& vector, std::minstd_rand& rng, real max_value);

}
