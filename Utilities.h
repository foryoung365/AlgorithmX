#pragma once

// 辅助函数，用于组合哈希值 (类似于 Boost 的 hash_combine)
template <class T>
inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// 递归模板，用于计算元组中每个元素的哈希值并组合
template <class Tuple, std::size_t Index = std::tuple_size<Tuple>::value - 1>
struct TupleHasher {
    static void hash_value(std::size_t &seed, const Tuple &tuple) {
        TupleHasher<Tuple, Index - 1>::hash_value(seed, tuple);
        hash_combine(seed, std::get<Index>(tuple));
    }
};

// 递归基本情况：当索引为0时
template <class Tuple>
struct TupleHasher<Tuple, 0> {
    static void hash_value(std::size_t &seed, const Tuple &tuple) {
        hash_combine(seed, std::get<0>(tuple));
    }
};

// 特化 std::hash 针对 std::tuple
namespace std {
template <typename... Ts>
struct hash<std::tuple<Ts...>> {
    std::size_t operator()(const std::tuple<Ts...> &t) const {
        std::size_t seed = 0;
        // 处理空元组的情况
        if constexpr (sizeof...(Ts) > 0) {
            TupleHasher<std::tuple<Ts...>>::hash_value(seed, t);
        }
        return seed;
    }
};
}  // namespace std

// 辅助函数，用于打印单个元素（可以根据需要特化）
template <typename T>
void print_element(const T &element) {
    std::cerr << element;
}

// 特化版本，用于打印字符串时加上引号
template <>
void print_element<std::string>(const std::string &element) {
    std::cerr << '"' << element << '"';
}

// 递归打印容器元素的辅助函数 (可选，用于元组中包含容器的情况)
template <typename T>
void print_container(const T &container) {
    std::cout << "{";
    bool first = true;
    for (const auto &item : container) {
        if (!first) {
            std::cout << ", ";
        }
        print_element(item);  // 递归调用，以处理容器内元素
        first = false;
    }
    std::cerr << "}";
}

// 主要的打印元组函数
template <typename... Args>
void print_tuple(const std::tuple<Args...> &t) {
    std::cerr << "(";
    bool first = true;
    std::apply(
        [&first](const auto &...args) {
            // 使用折叠表达式遍历元组元素
            auto print_with_comma = [&](const auto &arg) {
                if (!first) {
                    std::cerr << ", ";
                }
                print_element(arg);  // 使用辅助函数打印元素
                first = false;
            };
            (print_with_comma(args), ...);  // C++17 折叠表达式
        },
        t);
    std::cerr << ")" << std::endl;
}

void print_centered(const std::string &str, int total_width,
                    bool bNoTail = false, char fill_char = ' ') {
    int len = str.length();
    if (len >= total_width) {
        std::cout << str;  // 如果字符串长度超过或等于总宽度，直接输出
    } else {
        int padding_total = total_width - len;
        int padding_left = padding_total / 2;
        int padding_right = padding_total - padding_left;  // 处理奇数长度的填充

        std::cout << std::setfill(fill_char);
        std::cout << std::setw(padding_left) << "" << str;
        if (!bNoTail) {
            std::cout << std::setw(padding_right) << "";
        }

        std::cout << std::setfill(' ');  // 恢复默认填充字符为空格
    }
}