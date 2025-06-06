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

template <typename R>
class Solver {
public:
    using ActionID = R;
    using OnSolutionfound = std::function<void(const std::vector<ActionID>&)>;
    Solver(const std::vector<R>& requirements,
           const std::unordered_map<ActionID, std::vector<R>>& actions,
           const std::vector<R>& optionRequirements,
           bool bOnlyFoundOneSolution = false);
    virtual ~Solver();

    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;
    Solver(Solver&&) = delete;
    Solver& operator=(Solver&&) = delete;

    void Solve(OnSolutionfound foundCallback);

    const std::vector<ActionID>& GetSolution() const { return m_solution; }
    size_t GetSolutionCount() const { return m_solutionCount; }
    bool IsSolutionValid() const { return m_isSolutionValid; }

    void DumpMatrix();

protected:
    void selectRow(DlxCell<R>* row);
    void unselectRow(DlxCell<R>* row);

    static bool DefaultSortRequirement(Solver<R>* solver, const DlxCell<R>* a,
                                       const DlxCell<R>* b) {
        return solver->SortRequirement(a, b);
    }

    DlxCell<R>* AllocCell() { return &m_pAllocatedCells[m_nIdxAllocated++]; }

protected:
    virtual void remember(ActionID item);

    virtual void processRowSelection(DlxCell<R>* row);
    virtual void processRowUnselection(DlxCell<R>* row);
    virtual void processSolution();

    virtual bool SortAction(const DlxCell<R>* a, const DlxCell<R>* b) {
        return false;
    }

    virtual bool SortRequirement(const DlxCell<R>* a, const DlxCell<R>* b) {
        return a->getSize() < b->getSize();
    }

protected:
    std::vector<R> m_requirements;
    std::vector<R> m_optionRequirements;
    std::vector<ActionID> m_solution;
    size_t m_solutionCount = 0;
    std::stack<std::unordered_set<ActionID>> m_history;
    bool m_isSolutionValid = true;
    DlxCell<R>* m_matrixRoot = nullptr;
    std::vector<DlxCell<R>*> m_colHeaders;
    std::unordered_map<ActionID, DlxCell<R>*> m_rowHeaders;
    std::unordered_map<R, DlxCell<R>*> m_req2colHeader;
    size_t m_nonOptionalRequirementCount = 0;
    DlxCell<R>* m_pAllocatedCells = nullptr;
    size_t m_nIdxAllocated = 0;
    bool m_bOnlyOne = false;
};

template <typename R>
inline Solver<R>::Solver(
    const std::vector<R>& requirements,
    const std::unordered_map<ActionID, std::vector<R>>& actions,
    const std::vector<R>& optionRequirements, bool bOnlyFoundOneSolution) {
    m_requirements = requirements;
    m_nonOptionalRequirementCount = requirements.size();
    m_optionRequirements = optionRequirements;
    m_requirements.insert(m_requirements.end(), optionRequirements.begin(),
                          optionRequirements.end());
    m_bOnlyOne = bOnlyFoundOneSolution;

    // calculate cell total number
    int nTotal = 0;
    for (const auto& [k, v] : actions) {
        nTotal += v.size();
    }
    nTotal += actions.size();
    nTotal += m_requirements.size();
    nTotal += 1;  // for root

    m_pAllocatedCells = new DlxCell<R>[nTotal];

    m_matrixRoot = AllocCell();
    m_matrixRoot->setId(INT32_MAX);
    m_matrixRoot->setSize(INT32_MAX);

    for (int i = 0; i < m_requirements.size(); ++i) {
        auto& req = m_requirements[i];
        auto colHeader = AllocCell();
        colHeader->setId(i);
        m_colHeaders.emplace_back(colHeader);
        m_req2colHeader[req] = colHeader;
        m_matrixRoot->attach_horizontal(colHeader);
    }

    for (const auto& [actionID, reqByAction] : actions) {
        auto rowHeader = AllocCell();
        rowHeader->setValue(actionID);
        m_rowHeaders[actionID] = rowHeader;

        DlxCell<R>* prev = nullptr;
        for (const auto& req : reqByAction) {
            auto next = AllocCell();
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
    if (m_pAllocatedCells) {
        delete[] m_pAllocatedCells;
    }
}

template <typename R>
void Solver<R>::Solve(OnSolutionfound foundCallback) {
    if (m_matrixRoot == nullptr || m_colHeaders.empty() ||
        m_rowHeaders.empty()) {
        return;
    }

    auto bestColumn = m_matrixRoot;
    auto node = m_matrixRoot->getNextX();
    while (node && node != m_matrixRoot) {
        if (node->getSize() == 0) {
            return;
        }

        auto index = node->getId();
        if (index < m_nonOptionalRequirementCount) {
            if (bestColumn == m_matrixRoot ||
                DefaultSortRequirement(this, node, bestColumn)) {
                bestColumn = node;
                if (bestColumn->getSize() == 1) {
                    break;
                }
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
            return;
        }
    } else {
        std::vector<DlxCell<R>*> actions;
        node = bestColumn->getNextY();
        while (node && node != bestColumn) {
            actions.push_back(node);
            node = node->getNextY();
        }
        m_history.push(m_history.top());

        for (auto& action : actions) {
            selectRow(action);
            if (m_isSolutionValid && (!m_bOnlyOne || m_solutionCount == 0)) {
                this->Solve(foundCallback);
            }
            unselectRow(action);
            m_isSolutionValid = true;  // Reset validity for the next iteration
        }

        m_history.pop();
    }
}

template <typename R>
inline void Solver<R>::DumpMatrix() {
    for (auto node = m_matrixRoot->getNextX(); node != m_matrixRoot; node = node->getNextX()) {
        std::cout << node->getId() << ":" << node->getSize() << std::endl;
    }
}

template <typename R>
void Solver<R>::selectRow(DlxCell<R>* row) {
    if (!row) {
        return;
    }

    row->select();
    m_solution.push_back(row->getRowHeader()->getValue());
    this->processRowSelection(row);
}

template <typename R>
void Solver<R>::unselectRow(DlxCell<R>* row) {
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
inline void Solver<R>::processRowSelection(DlxCell<R>* row) {}

template <typename R>
void Solver<R>::processRowUnselection(DlxCell<R>* row) {}

template <typename R>
inline void Solver<R>::processSolution() {}
};  // namespace AlgorithmX