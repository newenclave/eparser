#pragma once
#include <functional>
#include <map>
#include <tuple>

#include "eparser/expressions/objects/base.h"

namespace eparser { namespace expressions { namespace objects {
    namespace oprerations {

        template <typename KeyT, typename ResultT = base::uptr>
        class binary {
        public:
            using key_type = KeyT;
            using result_type = ResultT;

            using function_type
                = std::function<result_type(base::ptr, base::ptr)>;
            using index_type
                = std::tuple<key_type, base::info::holder, base::info::holder>;
            using map_type = std::map<index_type, function_type>;

        private:
            template <typename LeftT, typename RightT, typename CallT>
            static function_type create_call(CallT fn)
            {
                return [fn](auto left, auto right) {
                    return fn(base::cast<LeftT>(left),
                              base::cast<RightT>(right));
                };
            }

            template <typename LeftT, typename RightT, typename CallT>
            static function_type create_reverse_call(CallT fn)
            {
                return [fn](auto left, auto right) {
                    return fn(base::cast<LeftT>(right),
                              base::cast<RightT>(left));
                };
            }

        public:
            template <typename LeftT, typename RightT, typename CallT>
            void set(key_type op, CallT call, bool add_revert = false)
            {
                if (!std::is_same<LeftT, RightT>::value && add_revert) {
                    set_impl(op, base::info::create<RightT>(),
                             base::info::create<LeftT>(),
                             create_reverse_call<LeftT, RightT>(call));
                }
                set_impl(op, base::info::create<LeftT>(),
                         base::info::create<RightT>(),
                         create_call<LeftT, RightT, CallT>(std::move(call)));
            }

            result_type call(key_type op, base::ptr left, base::ptr right)
            {
                if (auto fn = get(op, left, right)) {
                    return fn(left, right);
                }
                return {};
            }

            template <typename TargetObj>
            std::unique_ptr<TargetObj> call_cast(key_type op, base::ptr left,
                                                 base::ptr right)
            {
                return base::cast<TargetObj>(call(op, left, right));
            }

            function_type get(key_type op, base::ptr left, base::ptr right)
            {
                auto id = std::make_tuple(op, left->type_info(),
                                          right->type_info());
                auto find = bin_map_.find(id);
                if (find != bin_map_.end()) {
                    return find->second;
                }
                return {};
            }

            std::function<result_type()> wrap(key_type op, base::ptr left,
                                              base::ptr right)
            {
                if (auto fn = get(op, left, right)) {
                    return [fn, left, right]() { return fn(left, right); };
                }
                return {};
            }

            template <typename LeftT, typename RightT>
            function_type get(key_type op)
            {
                auto id = std::make_tuple(op, base::info::create<LeftT>(),
                                          base::info::create<RightT>());
                auto find = bin_map_.find(id);
                if (find != bin_map_.end()) {
                    return find->second;
                }
                return {};
            }

        private:
            void set_impl(key_type op, base::info::holder left_type,
                          base::info::holder right_type, function_type call)
            {
                bin_map_[std::make_tuple(op, left_type, right_type)]
                    = std::move(call);
            }

            map_type bin_map_;
        };

        template <typename KeyT, typename ResultT = base::uptr>
        class unary {
        public:
            using key_type = KeyT;
            using result_type = ResultT;
            using function_type = std::function<result_type(base::ptr)>;
            using index_type = std::tuple<key_type, base::info::holder>;
            using map_type = std::map<index_type, function_type>;

        private:
            template <typename ValueT, typename CallT>
            static function_type create_call(CallT fn)
            {
                return [fn](auto obj) { return fn(base::cast<ValueT>(obj)); };
            }

        public:
            template <typename ValueT, typename CallT>
            void set(key_type op, CallT fn)
            {
                set_impl(op, base::info::create<ValueT>(),
                         create_call<ValueT, CallT>(std::move(fn)));
            }

            result_type call(key_type op, base::ptr value)
            {
                if (auto fn = get(op, value)) {
                    return fn(value);
                }
                return {};
            }

            template <typename TargetObj>
            std::unique_ptr<TargetObj> call_cast(key_type op, base::ptr val)
            {
                return base::cast<TargetObj>(call(op, val));
            }

            function_type get(key_type op, base::ptr value)
            {
                auto id = std::make_tuple(op, value->type_info());
                auto find = un_map_.find(id);
                if (find != un_map_.end()) {
                    return find->second;
                }
                return {};
            }

            std::function<result_type()> wrap(key_type op, base::ptr value)
            {
                if (auto fn = get(op, value)) {
                    return [fn, value]() { return fn(value); };
                }
                return {};
            }

            template <typename ValueT>
            function_type get(key_type op)
            {
                auto id = std::make_tuple(op, base::info::create<ValueT>());
                auto find = un_map_.find(id);
                if (find != un_map_.end()) {
                    return find->second;
                }
                return {};
            }

        private:
            void set_impl(key_type op, base::info::holder value_type,
                          function_type call)
            {
                un_map_[std::make_tuple(op, value_type)] = std::move(call);
            }

        private:
            map_type un_map_;
        };

        class cast {
        public:
            using function_type = std::function<base::uptr(base::ptr)>;
            using index_type
                = std::tuple<base::info::holder, base::info::holder>;
            using map_type = std::map<index_type, function_type>;

        private:
            template <typename ValueT, typename CallT>
            static function_type create_unary_call(CallT fn)
            {
                return [fn](auto obj) { return fn(base::cast<ValueT>(obj)); };
            }

        public:
            template <typename FromT, typename ToT, typename CallT>
            void set(CallT fn)
            {
                set_impl(base::info::create<FromT>(), base::info::create<ToT>(),
                         create_unary_call<FromT>(std::move(fn)));
            }

            template <typename ToT>
            std::unique_ptr<ToT> call(base::ptr value)
            {
                auto id = std::make_tuple(value->type_info(),
                                          base::info::create<ToT>());
                auto find = trans_map_.find(id);
                if (find != trans_map_.end()) {
                    return base::cast<ToT>(find->second(value));
                }
                return {};
            }

            template <typename ToT>
            std::function<std::unique_ptr<ToT>(base::ptr)> get(base::ptr value)
            {
                auto id = std::make_tuple(value->type_info(),
                                          base::info::create<ToT>());
                auto find = trans_map_.find(id);
                if (find != trans_map_.end()) {
                    auto call = find->second;
                    return [call](auto value) {
                        return base::cast<ToT>(call(value));
                    };
                }
                return {};
            }

            template <typename ToT>
            std::function<std::unique_ptr<ToT>()> wrap(base::ptr value)
            {
                if (auto fn = get<ToT>(value)) {
                    return [fn, value]() { return fn(value); };
                }
                return {};
            }

            template <typename FromT, typename ToT>
            std::function<std::unique_ptr<ToT>(base::ptr)> get()
            {
                auto id = std::make_tuple(base::info::create<FromT>(),
                                          base::info::create<ToT>());
                auto find = trans_map_.find(id);
                if (find != trans_map_.end()) {
                    auto call = find->second;
                    return [call](auto value) {
                        return base::cast<ToT>(call(value));
                    };
                }
                return {};
            }

        private:
            void set_impl(base::info::holder from_type,
                          base::info::holder to_type, function_type call)
            {
                trans_map_[std::make_tuple(from_type, to_type)]
                    = std::move(call);
            }
            map_type trans_map_;
        };

        template <typename ResultT = base::uptr>
        class transform {
        public:
            using result_type = ResultT;
            using function_type = std::function<result_type(base::ptr)>;
            using index_type = base::info::holder;
            using map_type = std::map<index_type, function_type>;

        private:
            template <typename ValueT, typename CallT>
            static function_type create_unary_call(CallT fn)
            {
                return [fn](auto obj) { return fn(base::cast<ValueT>(obj)); };
            }

        public:
            template <typename FromT, typename CallT>
            void set(CallT fn)
            {
                auto id = base::info::create<FromT>();
                calls_[id] = create_unary_call<FromT>(std::move(fn));
            }

            result_type call(base::ptr value)
            {
                if (auto fn = get(value)) {
                    return fn(value);
                }
                return {};
            }

            function_type get(base::ptr value)
            {
                auto inf = value->type_info();
                auto itr = calls_.find(inf);
                if (itr != calls_.end()) {
                    return itr->second;
                }
                return {};
            }

            function_type wrap(base::ptr value)
            {
                if (auto fn = get(value)) {
                    return [fn, value]() { return fn(value); };
                }
                return {};
            }

            template <typename FromT>
            function_type get()
            {
                auto itr = calls_.find(base::info::create<FromT>());
                if (itr != calls_.end()) {
                    return itr->second;
                }
                return {};
            }

        private:
            map_type calls_;
        };

        template <typename KeyT, typename ResultT = base::uptr>
        class all {
        public:
            using key_type = KeyT;
            using result_type = ResultT;
            using binary_type = binary<KeyT, result_type>;
            using unary_type = unary<KeyT, result_type>;
            using cast_type = cast;
            using transfrom_type = transform<result_type>;

            binary_type& get_binary()
            {
                return binary_;
            }
            unary_type& get_unary()
            {
                return unary_;
            }
            cast_type& get_cast()
            {
                return cast_;
            }
            transfrom_type& get_transfrom()
            {
                return transfrom_;
            }

        private:
            binary_type binary_;
            unary_type unary_;
            cast_type cast_;
            transfrom_type transfrom_;
        };

}}}}
