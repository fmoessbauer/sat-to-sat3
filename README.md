# SAT to SAT-3 Converter

This program converts any CNF in a SAT-3 CNF, where each variable occurs at
most 3 times.

## Definition

Let $$SAT(k)$$ be SAT for CNF formulas with at most k occurrences of every variable.

## Lemma

SAT(k) for k >= 3 is NP-complete.

## SAT to SAT-3 reduction
The reduction is done by renaming all variables occuring more
than 3 times using a fresh variable. The equivalence is then ensured by
building a chain of implications between each new variable.
The last implication is build between the last "fresh" variable and the
original variable.

This reduction proves the given Lemma.

## Compiling

```
make release
```

## Running

- **Input:** Arbitrary CNF in [DIMACS](http://www.satcompetition.org/2009/format-benchmarks2009.html)
format (most SAT solvers use this format)
- **Output:** SAT-3 CNF in DIMACS format

There are two demo CNFs provided in the data directory.

## Implementation

There are two implementations differing only in the way the CNF is stored.

### Educational

This version stores the CNF by using STL containers selected in a way that
the characteristics of the container meet the properties of the problem.
Hence, for example each CNF-clause is stored as a set. The implementation
of the algorithm is straight forward and easy to understand. This is
probably how people which are new to C++ would implement it.

### Fast

The fast version uses a CSR-like format using two memory blocks. The first
block denotes the end-indices of the clauses in the second block.

This version is roughly 50 times faster than the educational implementation
and requires much less memory.

