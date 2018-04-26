#pragma once

#include <memory>
#include <mutex>

#include "args.h"
#include "vector.h"
#include "real.h"

namespace poincare {

static const real EPS = 1e-5;
static const real BOUNDARY = 1 - EPS;

class Model {
    protected:
        std::shared_ptr<std::vector<Vector>> vectors_;
        std::shared_ptr<Args> args_;
        real performance_;
        int64_t nexamples_;

        // these should be locals, but are instance variables to avoid the
        // (appreciable) slow sown resulting from repeated memory allocation
        std::vector<real> sample_sq_norms;
        std::vector<real> sample_sq_euc_dists;
        std::vector<real> arccosh_args;
        std::vector<real> activations;
        Vector acc_source_gradient;
        Vector tmp_gradient;

    public:
        Model(std::shared_ptr<std::vector<Vector>> vectors, std::shared_ptr<Args> args);

        void nickel_kiela_objective(int32_t source, std::vector<int32_t>& samples, real lr);

        /**
         * Return a metric on the average performance of this model since the last
         * call to this function (so this function is not idempotent).
         */
        real get_performance();

        /**
         * Update (in place) the poincare point in the direction of its
         * (poincare-)tangent vector `tangent`. 
         * Update by adding the tangent vector `tangent` and pulling back inside
         * the ball, if necessary (in the manner of Nickel & Kiela).
         */
        void update(Vector& point, const Vector& tangent);
};

inline real clip(real value, real min_, real max_) {
    if (value < min_) {
        return min_;
    } else if (value > max_) {
        return max_;
    }
    return value;
}

/**
 * Compute the poincare ball gradient of hyperbolic distance d(x, v) w.r.t. x,
 * given the various pre-computed scalars.
 */
inline void distance_gradient(Vector& gradient, const Vector& x, const Vector& v, real sqnormx, real sqnormv, real sqdist, real arccosh_arg) {
    gradient.zero();
    real alpha = 1 - sqnormx;
    real beta = 1 - sqnormv;
    real a = (sqnormv - 2 * dot(x, v) + 1) / pow(alpha, 2);
    gradient.add(x, a);
    gradient.add(v, -1 / alpha);
    real z = std::max(beta * sqrt(pow(arccosh_arg, 2) - 1), EPS);
    gradient.multiply(4. / z);
    // rescale the Euclidean gradient to obtain the Poincaré gradient
    gradient.multiply(pow(alpha, 2) / 4);
}

}
