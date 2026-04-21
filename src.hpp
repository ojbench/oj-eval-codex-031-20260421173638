// pylist: a Python-like list with reference semantics and int/list elements
#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_set>

class pylist {
    struct Node { std::vector<pylist> elems; };

    std::shared_ptr<Node> node_;   // non-null => this pylist is a list
    int value_ = 0;                // valid only when node_ == nullptr (scalar int)

    static void print_impl(std::ostream &os, const pylist &p,
                           std::unordered_set<const Node*> &stack) {
        if (!p.node_) { os << p.value_; return; }
        const Node *np = p.node_.get();
        if (stack.find(np) != stack.end()) { os << "[...]"; return; }
        stack.insert(np);
        os << "[";
        for (size_t i = 0; i < np->elems.size(); ++i) {
            if (i) os << ", ";
            print_impl(os, np->elems[i], stack);
        }
        os << "]";
        stack.erase(np);
    }

    // Ensure this object is a list (allocate node_ if needed)
    void ensure_list() {
        if (!node_) node_ = std::make_shared<Node>();
    }

public:
    // Constructors
    pylist() : node_(std::make_shared<Node>()), value_(0) {}
    pylist(int v) : node_(nullptr), value_(v) {}

    // Reference semantics come from copying shared_ptr when this is a list
    pylist(const pylist&) = default;
    pylist(pylist&&) noexcept = default;
    pylist &operator=(const pylist&) = default;
    pylist &operator=(pylist&&) noexcept = default;

    // Assign from int (turn into scalar)
    pylist &operator=(int v) { node_.reset(); value_ = v; return *this; }

    // Append int or pylist (O(1) amortized)
    void append(int v) {
        ensure_list();
        node_->elems.emplace_back(pylist(v));
    }
    void append(const pylist &x) {
        ensure_list();
        node_->elems.emplace_back(x); // copy; for lists shares node
    }

    // Pop last element (O(1) amortized). If empty, return empty list.
    pylist pop() {
        ensure_list();
        if (node_->elems.empty()) return pylist();
        pylist last = node_->elems.back();
        node_->elems.pop_back();
        return last;
    }

    // Index access (O(1)). Auto-resize with default elements (empty lists).
    pylist &operator[](size_t i) {
        ensure_list();
        if (i >= node_->elems.size()) node_->elems.resize(i + 1);
        return node_->elems[i];
    }
    const pylist &operator[](size_t i) const {
        // For const access, assume index is valid (per problem usage)
        return node_->elems[i];
    }

    // Implicit conversion to int for arithmetic/bitwise operations when scalar.
    // If used on a list, returns 0 (problem guarantees arithmetic only on ints).
    operator int() const { return node_ ? 0 : value_; }

    // Stream output
    friend std::ostream &operator<<(std::ostream &os, const pylist &p) {
        std::unordered_set<const Node*> stack;
        print_impl(os, p, stack);
        return os;
    }
};

#endif // PYLIST_H

