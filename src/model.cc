#include "model.h"

namespace poincare {

Model::Model(std::shared_ptr<std::vector<Vector>> vectors, std::shared_ptr<Args> args) :
    sample_sq_norms(args->number_negatives + 1),
    sample_sq_euc_dists(args->number_negatives + 1),
    arccosh_args(args->number_negatives + 1),
    activations(args->number_negatives + 1),
    acc_source_gradient(args->dimension),
    tmp_gradient(args->dimension) {
    vectors_ = vectors;
    args_ = args;
    performance_ = 0.0;
    nexamples_ = 1;
}

void Model::update(Vector& point, const Vector& tangent) {
    point.add(tangent);
    // pull back inside the ball if necessary
    real norm = std::sqrt(dot(point, point));
    if (norm >= 1) {
        point.multiply(1. / norm);
    }
}

void Model::nickel_kiela_objective(int32_t source, std::vector<int32_t>& samples, real lr) {
    real source_sq_norm = clip((vectors_->at(source)).squared_norm(), 0, BOUNDARY);
    real z = 0; // normalisation for the activations

    for (int32_t n = 0; n < samples.size(); n++) {
        sample_sq_euc_dists[n] = squared_dist(vectors_->at(source), vectors_->at(samples[n]));
        sample_sq_norms[n] = clip((vectors_->at(samples[n])).squared_norm(), 0, BOUNDARY);
        // note: we don't need to calculate the hyperbolic distance to calculate the activation,
        // since hyperbolic distance = arccosh(something) = ln(something_else) and activation = exp(-distance)
        // so can simplify using exp(-ln(x)) = 1 / x
        arccosh_args[n] = 1 + 2 * sample_sq_euc_dists[n] / ((1 - sample_sq_norms[n]) * (1 - source_sq_norm));
        real unnormed_activation = 1. / arccosh_args[n];
        activations[n] = unnormed_activation;
        z += unnormed_activation;
    }
    
    // normalise the activations
    for (int32_t n = 0; n < samples.size(); n++) {
        activations[n] /= z;
    }

    performance_ += activations[0];

    acc_source_gradient.zero();
    for (int32_t n = 0; n < samples.size(); n++) {
        real label = (n == 0);
        real weight = -label + activations[n];
        // compute the gradient of the source arising from the nth sample
        distance_gradient(tmp_gradient, vectors_->at(source),
                vectors_->at(samples[n]), source_sq_norm, sample_sq_norms[n],
                sample_sq_euc_dists[n], arccosh_args[n]);
        // accumulate the gradient for the source
        acc_source_gradient.add(tmp_gradient, weight);
        
        // compute gradient for the nth sample
        distance_gradient(tmp_gradient, vectors_->at(samples[n]),
                vectors_->at(source), sample_sq_norms[n], source_sq_norm,
                sample_sq_euc_dists[n], arccosh_args[n]);
        // update the output word vector
        tmp_gradient.multiply(lr * weight);
        update(vectors_->at(samples[n]), tmp_gradient);
    }
    nexamples_ += 1;

    acc_source_gradient.multiply(lr);
    update(vectors_->at(source), acc_source_gradient);
}


real Model::get_performance() {
    real avg = performance_ / nexamples_;
    performance_ = 0.0;
    nexamples_ = 1;
    return avg;
}

}
