#pragma once
#include <map>
#include <memory>

namespace eparser { namespace common {
    template <typename KeyT, typename ValueT, typename Comp = std::less<KeyT>>
    class trie {

        class NodeType {
        public:
            using key_type = KeyT;
            using value_type = ValueT;
            using value_ptr = std::unique_ptr<value_type>;
            using nodes_type = std::map<key_type, NodeType, Comp>;

            NodeType* get(const key_type& k)
            {
                auto f = m_next.find(k);
                return (f == m_next.end()) ? nullptr : &f->second;
            }

            NodeType* set(const key_type& k)
            {
                auto f = m_next.insert(std::make_pair(k, NodeType()));
                return &f.first->second;
            }

            void set_value(value_type val)
            {
                m_val.reset(new value_type(std::move(val)));
            }

            value_type* value()
            {
                return m_val.get();
            }

            const value_type* value() const
            {
                return m_val.get();
            }

        private:
            value_ptr m_val;
            nodes_type m_next;
        };

    public:
        using key_type = KeyT;
        using value_type = ValueT;
        using node_type = NodeType;

    public:
        trie() = default;
        trie(const trie&) = default;
        trie& operator=(const trie&) = default;
        trie(trie&&) = default;
        trie& operator=(trie&&) = default;

        template <typename Iter>
        class const_result_view {
        public:
            ~const_result_view() = default;
            const_result_view(const const_result_view&) = default;
            const_result_view(const_result_view&&) = default;
            const_result_view& operator=(const const_result_view&) = default;
            const_result_view& operator=(const_result_view&&) = default;
            const_result_view() = default;

            const_result_view(const node_type* node, Iter itrbegin, Iter itrend)
                : node_(node)
                , begin_(itrbegin)
                , end_(itrend)
            {
            }

            operator bool() const
            {
                return node_ != nullptr;
            }

            const value_type& operator*() const
            {
                return *node_->value();
            }

            const value_type* operator->() const
            {
                return node_->value();
            }

            Iter begin() const
            {
                return begin_;
            }

            Iter end() const
            {
                return end_;
            }

        private:
            const node_type* node_ = nullptr;
            Iter begin_;
            Iter end_;
        };

        template <typename Iter>
        class result_view {
        public:
            ~result_view() = default;
            result_view(const result_view&) = default;
            result_view(result_view&&) = default;
            result_view& operator=(const result_view&) = default;
            result_view& operator=(result_view&&) = default;

            result_view()
                : m_node(nullptr)
            {
            }

            result_view(node_type* node, Iter begin, Iter end)
                : m_node(node)
                , m_begin(begin)
                , m_end(end)
            {
            }

            operator bool() const
            {
                return m_node != nullptr;
            }

            value_type& operator*()
            {
                return *m_node->value();
            }

            const value_type& operator*() const
            {
                return *m_node->value();
            }

            value_type* operator->()
            {
                return m_node->value();
            }

            const value_type* operator->() const
            {
                return m_node->value();
            }

            Iter begin() const
            {
                return m_begin;
            }

            Iter end() const
            {
                return m_end;
            }

        private:
            node_type* m_node;
            Iter m_begin;
            Iter m_end;
        };

        template <typename IterT>
        void set(IterT begin, const IterT& end, value_type value)
        {
            set_s(&m_root, begin, end, std::move(value));
        }

        template <typename ContainerT>
        void set(const ContainerT& cnt, value_type value)
        {
            set(std::begin(cnt), std::end(cnt), std::move(value));
        }

        void set(const key_type* ptr, size_t len, value_type value)
        {
            set(ptr, ptr + len, std::move(value));
        }

        template <typename IterT>
        result_view<IterT> get(IterT b, const IterT& e, bool greedy)
        {
            return get_s<result_view>(&m_root, b, e, greedy);
        }

        template <typename IterT>
        const_result_view<IterT> get(IterT b, const IterT& e, bool greedy) const
        {
            return get_s<const_result_view>(&m_root, b, e, greedy);
        }

    private:
        template <typename IterT>
        static void set_s(node_type* last, IterT begin, const IterT& end,
                          value_type value)
        {
            for (; begin != end; ++begin) {
                last = last->set(*begin);
            }
            last->set_value(std::move(value));
        }

        template <template <typename> class ResultType, typename IterT,
                  typename node_type>
        static ResultType<IterT> get_s(node_type next_table, IterT b,
                                       const IterT& e, bool greedy)
        {
            using result_type = ResultType<IterT>;

            node_type last_final = nullptr;

            if (b == e) {
                return result_type(nullptr, b, e);
            }

            IterT start = b;
            IterT bb = b;
            ++bb;

            for (; b != e; ++b) {

                next_table = next_table->get(*b);

                if (!next_table) {
                    break;
                }

                if (next_table->value()) {

                    last_final = next_table;

                    bb = b;
                    ++bb;

                    if (!greedy) {
                        break;
                    }
                }
            }
            return last_final ? result_type(last_final, start, bb)
                              : result_type(nullptr, e, e);
        }
        node_type m_root;
    };

}}