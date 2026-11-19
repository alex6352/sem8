#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sem8 {
namespace {

using Count = std::uint64_t;
using Matrix = std::vector<std::vector<double>>;
constexpr Count kMaxSumInput = 1'000'000;

struct IntegerResult {
    Count value;
    Count operations;
};

struct RangeResult {
    double value;
    Count comparisons;
};

struct SymmetryResult {
    bool symmetric;
    Count comparisons;
};

struct ProductResult {
    Matrix value;
    Count multiplications;
    Count additions;
};

void require(bool condition, const char* error) {
    if (!condition) {
        throw std::invalid_argument(error);
    }
}

void check(bool condition) {
    if (!condition) {
        throw std::runtime_error("Experiment check failed");
    }
}

IntegerResult sum_squares(Count n) {
    require(n > 0 && n <= kMaxSumInput,
            "n must be in [1, 1000000]");
    IntegerResult result{0, 0};
    for (Count i = 1; i <= n; ++i) {
        result.value += i * i;
        ++result.operations;
    }
    return result;
}

IntegerResult sum_squares_formula(Count n) {
    require(n > 0 && n <= kMaxSumInput,
            "n must be in [1, 1000000]");
    Count result = n * (n + 1);
    Count multiplications = 1;
    result *= 2 * n + 1;
    multiplications += 2;
    return {result / 6, multiplications};
}

RangeResult find_range(const std::vector<double>& values) {
    require(!values.empty(), "Array must not be empty");
    double minimum = values.front();
    double maximum = minimum;
    Count comparisons = 0;
    for (std::size_t i = 1; i < values.size(); ++i) {
        const double value = values[i];
        ++comparisons;
        if (value < minimum) {
            minimum = value;
        }
        ++comparisons;
        if (value > maximum) {
            maximum = value;
        }
    }
    return {maximum - minimum, comparisons};
}

RangeResult find_range_pairs(const std::vector<double>& values) {
    require(!values.empty(), "Array must not be empty");
    double minimum = values.front();
    double maximum = minimum;
    Count comparisons = 0;
    std::size_t i = 1;
    if (values.size() % 2 == 0) {
        ++comparisons;
        if (values[0] < values[1]) {
            maximum = values[1];
        } else {
            minimum = values[1];
        }
        i = 2;
    }
    for (; i < values.size(); i += 2) {
        double smaller = values[i];
        double larger = values[i + 1];
        ++comparisons;
        if (larger < smaller) {
            smaller = values[i + 1];
            larger = values[i];
        }
        ++comparisons;
        if (smaller < minimum) {
            minimum = smaller;
        }
        ++comparisons;
        if (larger > maximum) {
            maximum = larger;
        }
    }
    return {maximum - minimum, comparisons};
}

std::size_t square_size(const Matrix& matrix) {
    const auto n = matrix.size();
    require(n > 0, "Matrix must not be empty");
    for (const auto& row : matrix) {
        require(row.size() == n, "Matrix must be square");
    }
    return n;
}

SymmetryResult check_symmetry(const Matrix& matrix) {
    const auto n = square_size(matrix);
    Count comparisons = 0;
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            ++comparisons;
            if (matrix[i][j] != matrix[j][i]) {
                return {false, comparisons};
            }
        }
    }
    return {true, comparisons};
}

ProductResult multiply(const Matrix& a, const Matrix& b,
                       bool skip_zero_addition) {
    const auto n = square_size(a);
    require(square_size(b) == n, "Matrix sizes must match");
    ProductResult result{Matrix(n, std::vector<double>(n)), 0,
                         0};
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            std::size_t k = 0;
            if (skip_zero_addition) {
                sum = a[i][0] * b[0][j];
                ++result.multiplications;
                k = 1;
            }
            for (; k < n; ++k) {
                sum += a[i][k] * b[k][j];
                ++result.multiplications;
                ++result.additions;
            }
            result.value[i][j] = sum;
        }
    }
    return result;
}

IntegerResult count_binary_digits(Count n) {
    require(n > 0, "Binary expects a positive number");
    IntegerResult result{1, 0};
    while (n > 1) {
        n /= 2;
        ++result.value;
        ++result.operations;
    }
    return result;
}

void run_mystery() {
    std::cout << "n | sum | loop multiplications | formula "
                 "multiplications\n";
    for (Count n : {1, 5, 10, 100, 1000}) {
        const auto loop = sum_squares(n);
        const auto formula = sum_squares_formula(n);
        check(loop.value == formula.value &&
              loop.operations == n && formula.operations == 3);
        std::cout << n << " | " << loop.value << " | "
                  << loop.operations << " | "
                  << formula.operations << '\n';
    }
}

void run_secret() {
    std::cout << "n | range | original comparisons | paired "
                 "comparisons\n";
    for (std::size_t n : {1, 2, 5, 6, 10, 100}) {
        std::vector<double> values(n);
        for (std::size_t i = 0; i < n; ++i) {
            values[i] = (i % 2 == 0 ? 1.0 : -1.0) *
                        static_cast<double>(i);
        }
        const auto original = find_range(values);
        const auto paired = find_range_pairs(values);
        check(original.value == paired.value &&
              original.comparisons == 2 * (n - 1) &&
              paired.comparisons == (3 * n + 1) / 2 - 2);
        std::cout << n << " | " << original.value << " | "
                  << original.comparisons << " | "
                  << paired.comparisons << '\n';
    }
}

void run_enigma() {
    std::cout << "n | symmetric comparisons | first mismatch "
                 "comparisons\n";
    for (std::size_t n : {2, 4, 8, 16, 32}) {
        Matrix matrix(n, std::vector<double>(n, 1.0));
        const auto symmetric = check_symmetry(matrix);
        matrix[0][1] = 2.0;
        const auto asymmetric = check_symmetry(matrix);
        check(symmetric.symmetric &&
              symmetric.comparisons == n * (n - 1) / 2 &&
              !asymmetric.symmetric &&
              asymmetric.comparisons == 1);
        std::cout << n << " | " << symmetric.comparisons << " | "
                  << asymmetric.comparisons << '\n';
    }
}

void run_multiplication() {
    std::cout << "n | multiplications | original additions | "
                 "improved additions\n";
    for (std::size_t n : {1, 2, 4, 8, 16}) {
        Matrix a(n, std::vector<double>(n));
        Matrix b(n, std::vector<double>(n));
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                a[i][j] = static_cast<double>(i + j + 1);
                b[i][j] = static_cast<double>(i + 2 * j + 1);
            }
        }
        const auto original = multiply(a, b, false);
        const auto improved = multiply(a, b, true);
        const Count cube = n * n * n;
        check(original.value == improved.value &&
              original.multiplications == cube &&
              improved.multiplications == cube &&
              original.additions == cube &&
              improved.additions == cube - n * n);
        std::cout << n << " | " << cube << " | "
                  << original.additions << " | "
                  << improved.additions << '\n';
    }
}

void run_binary() {
    std::cout << "n | bits | divisions\n";
    for (unsigned bits : {1, 2, 4, 8, 16, 32, 64}) {
        const Count n = Count{1} << (bits - 1);
        const auto result = count_binary_digits(n);
        check(result.value == bits &&
              result.operations == bits - 1);
        std::cout << n << " | " << result.value << " | "
                  << result.operations << '\n';
    }
}

}  // namespace

void run_task(int argc, char* argv[]) {
    const std::string task = argc == 2 ? argv[1] : "";
    constexpr std::array tasks{run_mystery, run_secret,
                               run_enigma, run_multiplication,
                               run_binary};
    require(task.size() == 1 && task[0] >= '1' && task[0] <= '5',
            "Usage: ./build/sem8 <1-5>");
    tasks[static_cast<std::size_t>(task[0] - '1')]();
    std::cout << "check OK\n";
}

}  // namespace sem8

int main(int argc, char* argv[]) {
    try {
        sem8::run_task(argc, argv);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
