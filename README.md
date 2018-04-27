# Poincaré Embeddings
A multi-threaded C++ implementation of Nickel &amp; Kiela's [Poincaré Embeddings for Learning Hierarchical Representations](https://papers.nips.cc/paper/7213-poincare-embeddings-for-learning-hierarchical-representations) paper from NIPS 2017.
We include the implementation details that first became clear with the release of the [implementation of the authors](https://github.com/facebookresearch/poincare-embeddings).  The advantage of this implementation is that we are able to make sparse updates and thereby train much more quickly.

As in the implementation of the authors, only the objective for embedding taxonomies is implemented.  Unlike that implementation, updates are not performed in mini-batches.

### Burn-in
To achieve burn-in, just train twice, initialising the second time with the vectors trained during the first time (i.e. during burn-in).  For example:

```
./poincare -graph wordnet/mammal_closure.tsv -number-negatives 2 -epochs 40 -output-vectors vectors-after-burnin.csv -start-lr 0.005 -end-lr 0.005 -distribution-power 1
./poincare -graph wordnet/mammal_closure.tsv -number-negatives 20 -epochs 500 -input-vectors vectors-after-burnin.csv -output-vectors vectors.csv -start-lr 0.5 -end-lr 0.5 -distribution-power 0
```

## Requirements

For evaluation of the embeddings, you'll need the Python 3 library scikit-learn.

## Installation

```
git clone git@github.com:lateral/poincare-embeddings.git
cd poincare-embeddings
cd build
cmake ../
make
```

## Usage

```
$ ./poincare
    -graph                      training file path
    -output-vectors             file path for trained vectors
    -input-vectors              file path for init vectors (optional)
    -start-lr                   start learning rate [0.5]
    -end-lr                     end learning rate [0.5]
    -dimension                  manifold dimension [10]
    -init-range                 range of components for uniform initialization [0.0001]
    -epochs                     number of epochs [5]
    -number-negatives           number of negatives sampled [10]
    -distribution-power         exponent to use to modify negative sampling distribution [1]
    -checkpoint-interval        save vectors every this many epochs [-1]
    -threads                    number of threads [4]
    -seed                       seed for the random number generator [1]
                                  n.b. only deterministic if single threaded!
```

## Training data

Training data is a two-column tab-separated CSV file without header.  The training files for the  WordNet hypernymy hierarchy and its mammal subtree and included in the `wordnet` folder.  These were derived as per the [implementation of the authors](https://github.com/facebookresearch/poincare-embeddings).

## Output format

Vectors are written out as a spaced-separated CSV without header, where the first column is the name of the node.

```
sen.n.01 -0.07573256650403173837 0.04804740830803629381
unit_of_measurement.n.01 -0.3194984358578525614 0.5269294142957902365
chorionic_villus_sampling.n.01 -0.1497520758532252668 0.01760349013420301248
assay.n.04 -0.3628120882612646686 0.05198792878325033239
egyptian.n.01 0.1210250472607995836 -0.01964832136051103934
...
```

## Evaluation

The script `evaluate` measures the performance of the trained embeddings:

```
$ ./evaluate --graph wordnet/noun_closure.tsv --vectors build/vectors.csv --sample-size 1000 --sample-seed 2 --include-map
Filename: build/vectors.csv
Random seed: 2
Using a sample of 1000 of the 82115 nodes.
65203 vectors are on the boundary (they will be pulled back).
mean rank:               286.23
mean precision@1:        0.3979
mean average precision:  0.5974
```

The mean average precision is not calculated by default since it is quite slow (you can turn it on with the option `--include-map`.  The precision@1 is calculated as a proxy (it's faster).
