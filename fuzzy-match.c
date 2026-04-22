#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *line;
    int distance;
    size_t max_len;
    size_t min_len;
    size_t common_substring_length;
    double score;
} LineScore;

static void freeLineScores(LineScore *lines, const size_t line_count) {
    for (size_t i = 0; i < line_count; i++) free(lines[i].line);
    free(lines);
}

static double damerauLevenshteinSimilarity(const int distance, const size_t max_len) {
    if (max_len == 0) return 1.0;

    return 1.0 - (double) distance / (double) max_len;
}

static bool equals(const char *s1, const char *s2, const size_t len) {
    for (size_t i = 0; i < len; i++) if (s1[i] != s2[i]) return false;

    return true;
}

static size_t maximalCommonSubstringLength(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    const size_t max_len = len1 > len2 ? len2 : len1;
    for (size_t len = max_len; len > 0; len--)
        for (size_t i = 0; i <= len1 - len; ++i)
            for (size_t j = 0; j <= len2 - len; ++j)
                if (equals(s1 + i, s2 + j, len)) return len;

    return 0;
}

static double maximalCommonSubstringSimilarity(const size_t common_substring_length, const size_t min_len) {
    if (min_len == 0) return 0.0;

    return (double) common_substring_length / (double) min_len;
}

int damerauLevenshteinDistance(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    const size_t width = len2 + 1;
    int *d = malloc((len1 + 1) * (len2 + 1) * sizeof(*d));
    if (d == NULL) {
        fprintf(stderr, "Failed to allocate distance matrix\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i <= len1; i++) d[i * width] = (int) i;
    for (size_t j = 0; j <= len2; j++) d[j] = (int) j;
    for (size_t i = 1; i <= len1; i++) {
        for (size_t j = 1; j <= len2; j++) {
            const int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            const int deletion = d[(i - 1) * width + j] + 1;
            const int insertion = d[i * width + (j - 1)] + 1;
            const int substitution = d[(i - 1) * width + (j - 1)] + cost;
            int best = deletion < insertion ? deletion : insertion;
            if (substitution < best)
                best = substitution;
            d[i * width + j] = best;
            if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] &&
                s1[i - 2] == s2[j - 1]) {
                const int transposition = d[(i - 2) * width + (j - 2)] + cost;
                if (transposition < d[i * width + j]) d[i * width + j] = transposition;
            }
        }
    }
    const int result = d[len1 * width + len2];
    free(d);

    return result;
}
int comparator(const void *a, const void *b) {
    auto const ls_a = (const LineScore *)a;
    auto const ls_b = (const LineScore *)b;

    if (ls_a->score < ls_b->score) return 1;
    if (ls_a->score > ls_b->score) return -1;
    if (ls_a->common_substring_length < ls_b->common_substring_length) return 1;
    if (ls_a->common_substring_length > ls_b->common_substring_length) return -1;
    if (ls_a->distance < ls_b->distance) return -1;
    if (ls_a->distance > ls_b->distance) return 1;

    return strcmp(ls_a->line, ls_b->line);
}

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <query>\n", argv[0]);
        return 1;
    }
    const char *query = argv[1];
    const size_t query_len = strlen(query);
    size_t line_count = 0;
    size_t capacity = 100;
    double max_similarity = 0.0;
    LineScore *lines = malloc(capacity * sizeof(*lines));
    if (lines == NULL) {
        fprintf(stderr, "Failed to allocate result buffer\n");
        return 1;
    }
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        if (line_count >= capacity) {
            capacity *= 2;
            LineScore *resized_lines = realloc(lines, capacity * sizeof(*lines));
            if (resized_lines == NULL) {
                fprintf(stderr, "Failed to grow result buffer\n");
                freeLineScores(lines, line_count);
                return 1;
            }
            lines = resized_lines;
        }
        const int distance = damerauLevenshteinDistance(query, buffer);
        const size_t max_len = query_len > len ? query_len : len;
        const size_t min_len = query_len > len ? len : query_len;
        const double score = damerauLevenshteinSimilarity(distance, max_len);
        lines[line_count].line = malloc(len + 1);
        if (lines[line_count].line == NULL) {
            fprintf(stderr, "Failed to allocate line buffer\n");
            freeLineScores(lines, line_count);
            return 1;
        }
        strcpy(lines[line_count].line, buffer);
        lines[line_count].distance = distance;
        lines[line_count].max_len = max_len;
        lines[line_count].min_len = min_len;
        lines[line_count].common_substring_length = 0;
        lines[line_count].score = score;
        if (line_count == 0 || score > max_similarity) {
            max_similarity = score;
        }
        line_count++;
    }

    if (max_similarity <= 0.5) {
        for (size_t i = 0; i < line_count; i++) {
            lines[i].common_substring_length = maximalCommonSubstringLength(query, lines[i].line);
            lines[i].score = maximalCommonSubstringSimilarity(lines[i].common_substring_length, lines[i].min_len);
        }
    }

    qsort(lines, line_count, sizeof(*lines), comparator);
    for (size_t i = 0; i < line_count; i++) {
        printf("%.4f\t%s\n", lines[i].score, lines[i].line);
    }
    freeLineScores(lines, line_count);

    return 0;
}
