#pragma once
#include <string>

namespace AlgorithmX {
template <typename T>
class DlxCell {
public:
    DlxCell() {}
    ~DlxCell() = default;
    DlxCell(const DlxCell&) = default;
    DlxCell& operator=(const DlxCell&) = default;
    DlxCell(DlxCell&&) = default;
    DlxCell& operator=(DlxCell&&) = default;

    DlxCell* getPrevX() const { return prev_x; }
    DlxCell* getNextX() const { return next_x; }
    DlxCell* getPrevY() const { return prev_y; }
    DlxCell* getNextY() const { return next_y; }
    DlxCell* getColHeader() const { return col_header; }
    void setColHeader(DlxCell* header) { col_header = header; }
    void setRowHeader(DlxCell* header) { row_header = header; }
    DlxCell* getRowHeader() const { return row_header; }
    const T& getValue() const { return value; }
    void setValue(const T& newValue) { value = newValue; }
    const int getId() const { return id; }
    void setId(int newId) { id = newId; }
    size_t getSize() const { return size; }
    void setSize(size_t newSize) { size = newSize; }

    void remove_x() {
        if (prev_x) prev_x->next_x = next_x;
        if (next_x) next_x->prev_x = prev_x;
    }

    void restore_x() {
        if (prev_x) prev_x->next_x = this;
        if (next_x) next_x->prev_x = this;
    }

    void remove_y() {
        if (prev_y) prev_y->next_y = next_y;
        if (next_y) next_y->prev_y = prev_y;
    }

    void restore_y() {
        if (prev_y) prev_y->next_y = this;
        if (next_y) next_y->prev_y = this;
    }

    void attach_horizontal(DlxCell* cell) {
        if (cell == nullptr) {
            return;
        }

        cell->prev_x = prev_x;
        cell->next_x = this;
        if (prev_x) prev_x->next_x = cell;
        prev_x = cell;
    }

    void attach_vertical(DlxCell* cell) {
        if (cell == nullptr) {
            return;
        }

        cell->prev_y = prev_y;
        cell->next_y = this;
        if (prev_y) prev_y->next_y = cell;
        prev_y = cell;
    }

    void remove_column() {
        this->remove_x();
        DlxCell* node = next_y;
        while (node != this) {
            node->remove_row();
            node = node->next_y;
        }
    }

    void restore_column() {
        DlxCell* node = prev_y;
        while (node != this) {
            node->restore_row();
            node = node->prev_y;
        }

        this->restore_x();
    }

    void remove_row() {
        DlxCell* node = next_x;
        while (node != this) {
            node->col_header->size -= 1;
            node->remove_y();
            node = node->next_x;
        }
    }

    void restore_row() {
        DlxCell* node = prev_x;
        while (node != this) {
            node->col_header->size += 1;
            node->restore_y();
            node = node->prev_x;
        }
    }

    void select() {
        DlxCell* node = this;
        do {
            node->remove_y();
            node->col_header->remove_column();
            node = node->next_x;
        } while (node != this);
    }

    void unselect() {
        DlxCell* node = this->prev_x;
        while (node != this) {
            node->col_header->restore_column();
            node->restore_y();
            node = node->prev_x;
            /* code */
        }
        node->col_header->restore_column();
        node->restore_y();
    }

private:
    DlxCell* prev_x = this;
    DlxCell* next_x = this;
    DlxCell* prev_y = this;
    DlxCell* next_y = this;

    DlxCell* col_header = nullptr;
    DlxCell* row_header = nullptr;

    T value;

    size_t size = 0;
    int id = 0;
};
};  // namespace AlgorithmX
