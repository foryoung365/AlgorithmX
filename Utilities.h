#pragma once

// ����������������Ϲ�ϣֵ (������ Boost �� hash_combine)
template <class T>
inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// �ݹ�ģ�壬���ڼ���Ԫ����ÿ��Ԫ�صĹ�ϣֵ�����
template <class Tuple, std::size_t Index = std::tuple_size<Tuple>::value - 1>
struct TupleHasher {
    static void hash_value(std::size_t &seed, const Tuple &tuple) {
        TupleHasher<Tuple, Index - 1>::hash_value(seed, tuple);
        hash_combine(seed, std::get<Index>(tuple));
    }
};

// �ݹ���������������Ϊ0ʱ
template <class Tuple>
struct TupleHasher<Tuple, 0> {
    static void hash_value(std::size_t &seed, const Tuple &tuple) {
        hash_combine(seed, std::get<0>(tuple));
    }
};

// �ػ� std::hash ��� std::tuple
namespace std {
template <typename... Ts>
struct hash<std::tuple<Ts...>> {
    std::size_t operator()(const std::tuple<Ts...> &t) const {
        std::size_t seed = 0;
        // �����Ԫ������
        if constexpr (sizeof...(Ts) > 0) {
            TupleHasher<std::tuple<Ts...>>::hash_value(seed, t);
        }
        return seed;
    }
};
}  // namespace std

// �������������ڴ�ӡ����Ԫ�أ����Ը�����Ҫ�ػ���
template <typename T>
void print_element(const T &element) {
    std::cerr << element;
}

// �ػ��汾�����ڴ�ӡ�ַ���ʱ��������
template <>
void print_element<std::string>(const std::string &element) {
    std::cerr << '"' << element << '"';
}

// �ݹ��ӡ����Ԫ�صĸ������� (��ѡ������Ԫ���а������������)
template <typename T>
void print_container(const T &container) {
    std::cout << "{";
    bool first = true;
    for (const auto &item : container) {
        if (!first) {
            std::cout << ", ";
        }
        print_element(item);  // �ݹ���ã��Դ���������Ԫ��
        first = false;
    }
    std::cerr << "}";
}

// ��Ҫ�Ĵ�ӡԪ�麯��
template <typename... Args>
void print_tuple(const std::tuple<Args...> &t) {
    std::cerr << "(";
    bool first = true;
    std::apply(
        [&first](const auto &...args) {
            // ʹ���۵����ʽ����Ԫ��Ԫ��
            auto print_with_comma = [&](const auto &arg) {
                if (!first) {
                    std::cerr << ", ";
                }
                print_element(arg);  // ʹ�ø���������ӡԪ��
                first = false;
            };
            (print_with_comma(args), ...);  // C++17 �۵����ʽ
        },
        t);
    std::cerr << ")" << std::endl;
}

void print_centered(const std::string &str, int total_width,
                    bool bNoTail = false, char fill_char = ' ') {
    int len = str.length();
    if (len >= total_width) {
        std::cout << str;  // ����ַ������ȳ���������ܿ�ȣ�ֱ�����
    } else {
        int padding_total = total_width - len;
        int padding_left = padding_total / 2;
        int padding_right = padding_total - padding_left;  // �����������ȵ����

        std::cout << std::setfill(fill_char);
        std::cout << std::setw(padding_left) << "" << str;
        if (!bNoTail) {
            std::cout << std::setw(padding_right) << "";
        }

        std::cout << std::setfill(' ');  // �ָ�Ĭ������ַ�Ϊ�ո�
    }
}