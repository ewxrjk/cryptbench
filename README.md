# cryptbench

This is a simple tool to measure the performance of crypt(3) on Linux.

## Installation

    make

## Use

To measure traditional DES-based crypt:

    ./cryptbench

To measure the more modern variants you must supply a suitable salt string.

    ./cryptbench --salt '$1$salt$' # MD5
    ./cryptbench --salt '$5$salt$' # SHA256
    ./cryptbench --salt '$6$salt$' # SHA512

## Interpretation

`cryptbench` will by default use all online CPU cores.
As such it provides a rough upper bound on the performance that
an online attacker may be able to extract from your system.
