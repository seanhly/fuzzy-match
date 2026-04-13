# fuzzy-match

A fast command-line tool for fuzzy string matching using the Damerau-Levenshtein distance algorithm.

## Features

- **Damerau-Levenshtein Distance**: Measures similarity between strings accounting for insertions, deletions, substitutions, and transpositions
- **Normalized Scoring**: Calculates similarity score as `distance / MAX(queryLength, lineLength)` for fair comparison regardless of string lengths
- **Sorted Output**: Results are sorted by similarity score (best matches first)
- **Efficient Processing**: Handles large input streams with dynamic memory allocation

## Building

### Prerequisites
- GCC compiler
- Make (optional)

### Compilation

Using Make:
```bash
make
```

Or directly with GCC:
```bash
gcc -o fuzzy-match fuzzy-match.c -lm
```

## Usage

```bash
fuzzy-match <query> < input.txt
```

or pipe input from another command:

```bash
echo -e "apple\napple pie\norange\nbanana\nappl" | fuzzy-match "apple"
```

### Output Format

Each line is printed with its similarity score (lower is more similar):

```
0.0000	apple
0.2000	appl
0.5000	apple pie
0.6667	banana
1.0000	orange
```

## Examples

### Basic matching
```bash
$ echo -e "cat\ncar\ndog\nhat" | fuzzy-match "cat"
0.0000	cat
0.3333	car
0.6667	hat
1.0000	dog
```

### Matching with typos
```bash
$ echo -e "programming\nprograming\nprogram\nprogamming" | fuzzy-match "programming"
0.0000	programming
0.0909	programing
0.1818	progamming
0.3333	program
```

## Algorithm

The program implements the **Damerau-Levenshtein distance** algorithm, which measures the minimum number of single-character edits (insertions, deletions, substitutions, and transpositions) needed to transform one string into another.

The similarity score is normalized to account for string length differences:
```
similarity_score = damerau_levenshtein_distance / MAX(query_length, line_length)
```

## Installation

Install to system PATH:
```bash
make install
```

Uninstall:
```bash
make uninstall
```

## Cleaning

Remove compiled binaries:
```bash
make clean
```

## License

This project is licensed under the GNU General Public License, version 3.
See `LICENSE` for the full text.

