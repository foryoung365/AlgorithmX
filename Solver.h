#pragma once
#include <functional>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DlxCell.h"
#include <memory>
#include <algorithm>

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
using SortAction = std::function<bool(const DlxCell*, const DlxCell*)>;
using SortRequirement = std::function<bool(const DlxCell*, const DlxCell*)>;
using OnSolutionfound = std::function<void(const std::vector<ActionID>&)>;

template <typename R>
class Solver {
   public:
    Solver(const std::vector<R>& requirements,
           const std::unordered_map<ActionID, std::vector<R>>& actions,
           const std::unordered_set<R>& optionRequirements);
    virtual ~Solver();

    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;
    Solver(Solver&&) = delete;
    Solver& operator=(Solver&&) = delete;

    std::vector<ActionID> Solve(OnSolutionfound foundCallback);

    const std::vector<ActionID>& GetSolution() const { return m_solution; }
    size_t GetSolutionCount() const { return m_solutionCount; }
    bool IsSolutionValid() const { return m_isSolutionValid; }

   protected:
    void selectRow(DlxCell* row);
    void unselectRow(DlxCell* row);

    virtual void remember(ActionID item);

    virtual void processRowSelection(DlxCell* row);
    virtual void processRowUnselection(DlxCell* row);
    virtual void processSolution();

    static bool defaultSortAction(const DlxCell* a, const DlxCell* b) {
        return false;
    }

    static bool defaultSortRequirement(const DlxCell* a, const DlxCell* b) {
        return a->getColHeader()->getSize() < b->getColHeader()->getSize();
    }

   protected:
    std::vector<R> m_requirements;
    std::unordered_map<ActionID, std::vector<R>> m_actions;
    std::unordered_set<R> m_optionRequirements;
    std::vector<ActionID> m_solution;
    size_t m_solutionCount = 0;
    std::stack<std::unordered_set<ActionID>> m_history;
    bool m_isSolutionValid = true;
    DlxCell* m_matrixRoot = nullptr;
    std::vector<DlxCell*> m_colHeaders;
    std::unordered_map<ActionID, DlxCell*> m_rowHeaders;
    std::unordered_map<R, DlxCell*> m_req2colHeader;
    size_t m_nonOptionalRequirementCount = 0;
};

template <typename R>
inline Solver<R>::Solver(
    const std::vector<R>& requirements,
    const std::unordered_map<ActionID, std::vector<R>>& actions,
    const std::unordered_set<R>& optionRequirements) {
    m_requirements = requirements;
    m_nonOptionalRequirementCount = requirements.size();
    m_actions = actions;
    m_optionRequirements = optionRequirements;
    m_requirements.insert(m_requirements.end(), optionRequirements.begin(),
                          optionRequirements.end());
    m_matrixRoot = new DlxCell("Root");
    m_matrixRoot->setSize(INT32_MAX);

    for (int i = 0; i < m_requirements.size(); ++i) {
        auto& req = m_requirements[i];
        auto colHeader =new DlxCell(std::to_string(i));
        m_colHeaders.emplace_back(colHeader);
        m_req2colHeader[req] = colHeader;
        m_matrixRoot->attach_horizontal(colHeader);
    }

    for (const auto& [actionID, reqByAction] : m_actions) {
        auto rowHeader = new DlxCell(actionID);
        m_rowHeaders[actionID] = rowHeader;

        DlxCell* prev = nullptr;
        for (const auto& req : reqByAction) {
            auto next = new DlxCell();
            next->setColHeader(m_req2colHeader[req]);
            next->setRowHeader(rowHeader);
            next->getColHeader()->setSize(next->getColHeader()->getSize() + 1);
            next->getColHeader()->attach_vertical(next);
            if (prev) {
                prev->attach_horizontal(next);
            } else {
                prev = next;
            }
        }
    }

    m_history.push(std::unordered_set<ActionID>());
}

template <typename R>
Solver<R>::~Solver() {
    std::vector<DlxCell*> toDelete;
    toDelete.push_back(m_matrixRoot);
    DlxCell* node = m_matrixRoot->getNextX();
    while (node != m_matrixRoot) {
        toDelete.push_back(node);
        DlxCell* verticalNode = node->getNextY();
        while (verticalNode != node) {
            toDelete.push_back(verticalNode);
            verticalNode = verticalNode->getNextY();
        }
        node = node->getNextX();
    }

    for (auto cell : toDelete) {
        delete cell;
    }
    toDelete.clear();
}

template <typename R>
std::vector<ActionID> Solver<R>::Solve(OnSolutionfound foundCallback) {
    if (m_matrixRoot == nullptr || m_colHeaders.empty() ||
        m_rowHeaders.empty()) {
        return {};
    }

    auto bestColumn = m_matrixRoot;
    auto node = m_matrixRoot->getNextX();
    while (node != m_matrixRoot) {
        auto index = std::stoll(node->getTitle());
        if (index < m_nonOptionalRequirementCount) {
            if (defaultSortRequirement(node, bestColumn)) {
                bestColumn = node;
            }
            node = node->getNextX();
        } else {
            node = m_matrixRoot;
        }
    }

    if (bestColumn == m_matrixRoot) {
        processSolution();
        if (m_isSolutionValid) {
            m_solutionCount++;
            if (foundCallback) {
                foundCallback(m_solution);
            }
        }
    } else {
        std::vector<DlxCell*> actions;
        node = bestColumn->getNextY();
        while (node != bestColumn) {
            actions.push_back(node);
            node = node->getNextY();
        }
        m_history.push(m_history.top());

        std::sort(actions.begin(), actions.end(), defaultSortAction);
        for (auto& action : actions) {
            selectRow(action);
            if (m_isSolutionValid) {
                this->Solve(foundCallback);
            }
            unselectRow(action);
            m_isSolutionValid = true;  // Reset validity for the next iteration
        }

        m_history.pop();
    }
}

template <typename R>
void Solver<R>::selectRow(DlxCell* row) {
    if (!row) {
        return;
    }

    row->select();
    m_solution.push_back(row->getRowHeader()->getTitle());
    this->processRowSelection(row->getRowHeader());
}

template <typename R>
void Solver<R>::unselectRow(DlxCell* row) {
    if (!row) {
        return;
    }

    row->unselect();
    m_solution.pop_back();
    this->processRowUnselection(row->getRowHeader());
}

template <typename R>
inline void Solver<R>::remember(ActionID item) {
    if (m_history.top().find(item) == m_history.top().end()) {
        m_history.top().insert(item);
    } else {
        m_isSolutionValid = false;
    }
}

template <typename R>
inline void Solver<R>::processRowSelection(DlxCell* row) {}

template <typename R>
void Solver<R>::processRowUnselection(DlxCell* row) {}

template <typename R>
inline void Solver<R>::processSolution() {}
};  // namespace AlgorithmX