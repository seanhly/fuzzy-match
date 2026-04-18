# fuzzy-match

![Logo](./docs/icons/512x512.png)

A fast command-line tool for fuzzy string matching using the Damerau-Levenshtein distance algorithm, with a longest-common-substring fallback when no strong match is found.

## Features

- **Damerau-Levenshtein Distance**: Measures similarity between strings accounting for insertions, deletions, substitutions, and transpositions
- **Normalized Scoring**: Calculates similarity score as `1 - distance / MAX(queryLength, lineLength)` so higher scores are better
- **Fallback Matching**: If the best Damerau-Levenshtein similarity is below `0.5`, recalculates every score using the maximal common substring length
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

Each line is printed with its similarity score (higher is more similar):

```
1.0000	apple
0.8000	appl
0.5556	apple pie
0.1667	banana
0.1667	orange
```

## Examples

### Basic matching
```bash
$ echo -e "cat\ncar\ndog\nhat" | fuzzy-match "cat"
1.0000	cat
0.6667	car
0.6667	hat
0.0000	dog
```

### Matching with typos
```bash
$ echo -e "programming\nprograming\nprogram\nprogamming" | fuzzy-match "programming"
1.0000	programming
0.9091	programing
0.9091	progamming
0.6364	program
```

### Fallback to maximal common substring
If no Damerau-Levenshtein similarity reaches `0.5`, every score is recalculated using the longest common substring length instead.

## Algorithm

The program first computes a **Damerau-Levenshtein similarity**, based on the minimum number of single-character edits (insertions, deletions, substitutions, and transpositions) needed to transform one string into another.

The primary similarity score is normalized to account for string length differences:
```
similarity_score = 1 - damerau_levenshtein_distance / MAX(query_length, line_length)
```

If the highest primary similarity is below `0.5`, the program recalculates every score using the maximal common substring length instead:
```
similarity_score = longest_common_substring_length / MAX(query_length, line_length)
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

