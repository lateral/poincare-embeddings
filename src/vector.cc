#include "vector.h"

#include <random>
#include <cmath>
#include <assert.h>

#include <iomanip>
#include <cmath>
#include <limits>

namespace poincare {

    Vector::Vector(int64_t m) {
        dimension_ = m;
        data_ = new real[m];
        zero();
    }

    Vector::Vector(const Vector& v) {
        dimension_ = v.dimension_;
        data_ = new real[dimension_];
        for (int64_t i = 0; i < dimension_; ++i) {
            data_[i] = v[i];
        }
    }

    Vector& Vector::operator=(const Vector& v) {
        delete[] data_;
        dimension_ = v.dimension_;
        data_ = new real[dimension_];
        for (int64_t i = 0; i < dimension_; ++i) {
            data_[i] = v[i];
        }
        return *this;
    }

    Vector::~Vector() {
        delete[] data_;
    }

    int64_t Vector::size() const {
        return dimension_;
    }

    void Vector::zero() {
        for (int64_t i = 0; i < dimension_; i++) {
            data_[i] = 0.0;
        }
    }

    void Vector::multiply(real a) {
        for (int64_t i = 0; i < dimension_; i++) {
            data_[i] *= a;
        }
    }

    void Vector::add(const Vector& source) {
        assert(dimension_ == source.dimension_);
        for (int64_t i = 0; i < dimension_; i++) {
            data_[i] += source.data_[i];
        }
    }

    void Vector::add(const Vector& source, real s) {
        assert(dimension_ == source.dimension_);
        for (int64_t i = 0; i < dimension_; i++) {
            data_[i] += s * source.data_[i];
        }
    }

    real& Vector::operator[](int64_t i) {
        return data_[i];
    }

    const real& Vector::operator[](int64_t i) const {
        return data_[i];
    }

    std::ostream& operator<<(std::ostream& os, const Vector& v) {
        os.precision(std::numeric_limits<real>::digits10 + 1);
        for (int64_t j = 0; j < v.dimension_ - 1; j++) {
            os << v.data_[j] << ' ';
        }
        os << v.data_[v.dimension_ - 1];
        return os;
    }

    void random_uniform_components(Vector& vector, std::minstd_rand& rng, real max_value) {
        std::uniform_real_distribution<> dist(-max_value, max_value);
        for (int64_t j = 0; j < vector.size(); ++j) {
            vector[j] = dist(rng);
        }
    }

    real Vector::squared_norm() const {
        real res = 0;
        for (int32_t i = 0; i < size(); i++) {
            res += pow(data_[i], 2);
        }
        return res;
    }
}
