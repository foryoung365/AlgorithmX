#include "Solver.h"

#include <algorithm>

namespace AlgorithmX {
template <typename R>
inline Solver<R>::Solver(const std::vector<R>& requirements,
                            const std::unordered_map<ActionID, std::vector<R>>& actions,
                            const std::unordered_set<R>& optionRequirements) {
    m_requirements = requirements;
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
            next->setColHeader = m_req2colHeader[req];
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
}
};  // namespace AlgorithmX