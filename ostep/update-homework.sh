#!/usr/bin/env bash

set -eu

wget https://github.com/remzi-arpacidusseau/ostep-homework/archive/refs/heads/master.zip
unzip master.zip
rm master.zip*
rsync -a ostep-homework-master/ homework/
rm -rf ostep-homework-master
