#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DlxCell.h"
#include "Utilities.h"

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

    bool Solve(OnSolutionfound foundCallback);

    const std::vector<ActionID>& GetSolution() const { return m_solution; }
    size_t GetSolutionCount() const { return m_solutionCount; }
    bool IsSolutionValid() const { return m_isSolutionValid; }

    void DumpMatrix();

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
        return a->getSize() < b->getSize();
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
        auto colHeader = new DlxCell(std::to_string(i));
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
    while (node && node != m_matrixRoot) {
        toDelete.push_back(node);
        DlxCell* verticalNode = node->getNextY();
        while (verticalNode != node) {
            toDelete.push_back(verticalNode);
            verticalNode = verticalNode->getNextY();
        }
        node = node->getNextX();
    }

    for (auto [actionId, rowHeader] : m_rowHeaders) {
        toDelete.push_back(rowHeader);
    }

    for (auto cell : toDelete) {
        delete cell;
    }
    toDelete.clear();
}

template <typename R>
bool Solver<R>::Solve(OnSolutionfound foundCallback) {
    if (m_matrixRoot == nullptr || m_colHeaders.empty() ||
        m_rowHeaders.empty()) {
        return false;
    }

    auto bestColumn = m_matrixRoot;
    auto node = m_matrixRoot->getNextX();
    while (node && node != m_matrixRoot) {
        auto index = std::stoll(node->getTitle());
        if (index < m_nonOptionalRequirementCount) {
            if (bestColumn == m_matrixRoot ||
                defaultSortRequirement(node, bestColumn)) {
                bestColumn = node;
            }
            node = node->getNextX();
        } else {
            node = m_matrixRoot;
        }
    }

    std::cerr << "best Column:" << bestColumn->getTitle() << std::endl;

    if (bestColumn == m_matrixRoot) {
        processSolution();
        if (m_isSolutionValid) {
            m_solutionCount++;
            if (foundCallback) {
                foundCallback(m_solution);
            }
            return true;
        }
    } else {
        std::vector<DlxCell*> actions;
        node = bestColumn->getNextY();
        while (node && node != bestColumn) {
            actions.push_back(node);
            node = node->getNextY();
        }
        m_history.push(m_history.top());

        std::sort(actions.begin(), actions.end(), defaultSortAction);
        for (auto& action : actions) {
            std::cerr << "try action:" << action->getRowHeader()->getTitle()
                      << std::endl;
            selectRow(action);
            if (m_isSolutionValid) {
                this->Solve(foundCallback);
            }
            std::cerr << "undo action:" << action->getRowHeader()->getTitle()
                      << std::endl;
            unselectRow(action);
            m_isSolutionValid = true;  // Reset validity for the next iteration
        }

        m_history.pop();
    }

    return false;
}

template <typename R>
inline void Solver<R>::DumpMatrix() {

    for (auto [id, header] : m_rowHeaders) {
        std::cerr << id << "\t";
        for (auto col = header->getNextX(); col != header; col = col->getNextX()) {
            std::cerr << col->getColHeader()->getTitle() << ":" << 1 << "\t";
        }
        std::cerr << std::endl;
    }
}

template <typename R>
void Solver<R>::selectRow(DlxCell* row) {
    if (!row) {
        return;
    }

    row->select();
    m_solution.push_back(row->getRowHeader()->getTitle());
    this->processRowSelection(row);
}

template <typename R>
void Solver<R>::unselectRow(DlxCell* row) {
    if (!row) {
        return;
    }

    row->unselect();
    m_solution.pop_back();
    this->processRowUnselection(row);
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