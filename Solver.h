#pragma once
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DlxCell.h"

// 辅助函数，用于组合哈希值 (类似于 Boost 的 hash_combine)
template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// 递归模板，用于计算元组中每个元素的哈希值并组合
template <class Tuple, std::size_t Index = std::tuple_size<Tuple>::value - 1>
struct TupleHasher {
    static void hash_value(std::size_t& seed, const Tuple& tuple) {
        TupleHasher<Tuple, Index - 1>::hash_value(seed, tuple);
        hash_combine(seed, std::get<Index>(tuple));
    }
};

// 递归基本情况：当索引为0时
template <class Tuple>
struct TupleHasher<Tuple, 0> {
    static void hash_value(std::size_t& seed, const Tuple& tuple) {
        hash_combine(seed, std::get<0>(tuple));
    }
};

// 特化 std::hash 针对 std::tuple
namespace std {
template <typename... Ts>
struct hash<std::tuple<Ts...>> {
    std::size_t operator()(const std::tuple<Ts...>& t) const {
        std::size_t seed = 0;
        // 处理空元组的情况
        if constexpr (sizeof...(Ts) > 0) {
            TupleHasher<std::tuple<Ts...>>::hash_value(seed, t);
        }
        return seed;
    }
};
}  // namespace std

namespace AlgorithmX {
using ActionID = std::string;
template <typename R>
class Solver {
   public:
    Solver(const std::vector<R>& requirements,
           const std::unordered_map<ActionID, std::vector<R>>& actions,
           const std::unordered_set<R>& optionRequirements);

    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;
    Solver(Solver&&) = delete;
    Solver& operator=(Solver&&) = delete;

    void Solve();

    

   private:
    std::vector<R> m_requirements;
    std::unordered_map<ActionID, A> m_actions;
    std::unordered_set<R> m_optionRequirements;
    std::vector<ActionID> m_solution;
    size_t m_solutionCount = 0;
    std::stack<std::set<A>> m_history;
    bool m_solutionIsValid = true;
    DlxCell* m_matrixRoot = nullptr;
    std::vector<DlxCell*> m_colHeaders;
    std::unordered_map<ActionID, DlxCell*> m_rowHeaders;
    std::unordered_map<R, DlxCell*> m_req2colHeader;
};
};