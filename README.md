# Author Identification

## Short Description

This program identifies the most likely authors of an anonymous sample text given a database of text with known authors. The program uses the k-Nearest Neighbors algorithm (KNN) to classify samples of text and determine which authors are most likely to have written the anonymous sample of the text.

## Build:

make all<br/>
make identify<br/>

## Running:

./identify OPTIONS<br/>

## Options:

    -d              Database of authors and texts [default: lib.db]
    -k matches      Set number of top matches to display [default: 5]
    -n noisefile    Set file of words to ignore [default: noise.txt]
    -l limit        Set noise word limit [default: 100]
    -e              Set distance metric as Euclidean distance [default]
    -m              Set distance metric as Manhattan distance
    -m              Set distance metric as cosine distance
    -h              Display program help and usage

## Formatting:

make format

## Cleaning:

make clean

## Errors:

No known bugs or errors.
