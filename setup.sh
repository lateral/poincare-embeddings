cd build
cmake ../
make -j8

##########################
# Run example
#########################

touch vectors-after-burnin.csv

./poincare -graph wordnet/mammal_closure.tsv -number-negatives 20 -epochs 500 -input-vectors vectors-after-burnin.csv -output-vectors vectors.csv -start-lr 0.5 -end-lr 0.5 -distribution-power 1

touch vectors.csv
./evaluate --graph wordnet/noun_closure.tsv --vectors build/vectors.csv --sample-size 1000 --sample-seed 2 --include-map
