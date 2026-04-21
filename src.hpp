// pylist: a Python-like list with reference semantics and int/list elements
#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_set>

class pylist {
    struct Node { std::vector<pylist> elems; };

    std::shared_ptr<Node> node_;   // owning list pointer (when this is a list)
    Node* alias_ = nullptr;        // non-owning pointer (used for self-references to avoid cycles)
    int value_ = 0;                // valid only when representing a scalar int (node_==nullptr && alias_==nullptr)
    Node* parent_ = nullptr;       // back-pointer to the parent Node when stored inside a list

    static void print_impl(std::ostream &os, const pylist &p,
                           std::unordered_set<const Node*> &stack) {
        const Node *np = p.get_node();
        if (!np) { os << p.value_; return; }
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
        if (!get_node()) node_ = std::make_shared<Node>();
    }

    Node* get_node() const { return alias_ ? alias_ : node_.get(); }

public:
    // Constructors
    pylist() : node_(std::make_shared<Node>()), alias_(nullptr), value_(0), parent_(nullptr) {}
    pylist(int v) : node_(nullptr), alias_(nullptr), value_(v), parent_(nullptr) {}

    // Reference semantics come from copying shared_ptr when this is a list
    pylist(const pylist&) = default;
    pylist(pylist&&) noexcept = default;
    pylist &operator=(pylist&&) noexcept = default;

    // Assign from int (turn into scalar)
    pylist &operator=(int v) { node_.reset(); alias_ = nullptr; value_ = v; return *this; }

    // Assign from pylist; detect self-referential assignment when inside a parent
    pylist &operator=(const pylist &rhs) {
        if (this == &rhs) return *this;
        // If assigning a list into an element that belongs to that same list, store non-owning alias
        Node* rhs_node = rhs.get_node();
        if (parent_ && rhs_node == parent_) {
            node_.reset(); alias_ = parent_; value_ = 0; // becomes alias to parent list
            return *this;
        }
        // Otherwise copy scalar or share list normally
        if (rhs_node) {
            node_ = rhs.node_; alias_ = rhs.alias_; value_ = 0;
        } else {
            node_.reset(); alias_ = nullptr; value_ = rhs.value_;
        }
        return *this;
    }

    // Append int or pylist (O(1) amortized)
    void append(int v) {
        ensure_list();
        Node* np = get_node();
        np->elems.emplace_back(pylist(v));
        np->elems.back().parent_ = np;
    }
    void append(const pylist &x) {
        ensure_list();
        Node* np = get_node();
        pylist e = x;
        // If appending itself (direct self-reference), break ownership to avoid cycle
        if (e.get_node() == np) { e.node_.reset(); e.alias_ = np; }
        e.parent_ = np;
        np->elems.emplace_back(e);
    }

    // Pop last element (O(1) amortized). If empty, return empty list.
    pylist pop() {
        ensure_list();
        Node* np = get_node();
        if (np->elems.empty()) return pylist();
        pylist last = np->elems.back();
        np->elems.pop_back();
        last.parent_ = nullptr; // detach from parent
        return last;
    }

    // Index access (O(1)). Auto-resize with default elements (empty lists).
    pylist &operator[](size_t i) {
        ensure_list();
        Node* np = get_node();
        if (i >= np->elems.size()) {
            size_t old = np->elems.size();
            np->elems.resize(i + 1);
            for (size_t k = old; k < np->elems.size(); ++k) {
                np->elems[k].parent_ = np;
            }
        }
        return np->elems[i];
    }
    const pylist &operator[](size_t i) const {
        // For const access, assume index is valid (per problem usage)
        const Node* np = get_node();
        return np->elems[i];
    }

    // Implicit conversion to int for arithmetic/bitwise operations when scalar.
    // If used on a list, returns 0 (problem guarantees arithmetic only on ints).
    operator int() const { return get_node() ? 0 : value_; }

    // Stream output
    friend std::ostream &operator<<(std::ostream &os, const pylist &p) {
        std::unordered_set<const Node*> stack;
        print_impl(os, p, stack);
        return os;
    }
};

#endif // PYLIST_H
