#pragma once

#include <cstdlib>
#include <functional>
#include <memory>
#include <type_traits>

namespace eparser { namespace expressions { namespace objects {

    struct base {
    public:
        using ptr = base*;
        using cptr = const base*;
        using uptr = std::unique_ptr<base>;
        using sptr = std::shared_ptr<base>;

        class info {
        public:
            class holder {
            public:
                holder(const info* i)
                    : info_(i)
                {
                }
                holder(holder&&) = default;
                holder& operator=(holder&&) = default;
                holder(const holder&) = default;
                holder& operator=(const holder&) = default;

                const info* type_info() const
                {
                    return info_;
                }
                const info* operator->() const
                {
                    return info_;
                }

            private:
                const info* info_;
            };
            info(std::uintptr_t i, std::function<base::uptr()> factory)
                : id_(i)
                , factory_(std::move(factory))
            {
            }

            base::uptr create_instance() const
            {
                return factory_();
            }

            template <typename T>
            static holder create()
            {
                return create_impl<T>(std::is_abstract<T> {});
            }

            std::uintptr_t id() const
            {
                return id_;
            }

        private:
            template <typename T>
            static holder create_impl(std::false_type)
            {
                static_assert(std::is_base_of<base, T>::value,
                              "T must derive from 'objects::base'");
                static std::uintptr_t localid
                    = static_cast<std::uintptr_t>(0xFFEEBBAA);
                static info sinfo(reinterpret_cast<std::uintptr_t>(&localid),
                                  []() { return std::make_unique<T>(); });
                return holder { &sinfo };
            }

            template <typename T>
            static holder create_impl(std::true_type)
            {
                static_assert(std::is_base_of<base, T>::value,
                              "T must derive from 'objects::base'");
                static std::uintptr_t localid
                    = static_cast<std::uintptr_t>(0xAABBCCDD);
                static info sinfo(reinterpret_cast<std::uintptr_t>(&localid),
                                  []() -> std::unique_ptr<T> {
                                      throw std::runtime_error(
                                          "Unable to create abstract type");
                                  });
                return holder { &sinfo };
            }
            std::uintptr_t id_ = 0;
            std::function<base::uptr()> factory_;
        };

        base(info::holder inf)
            : info_(std::move(inf))
        {
        }

        virtual ~base() = default;

        template <typename ToT>
        static const ToT* cast(cptr p)
        {
            static_assert(std::is_base_of<base, ToT>::value,
                          "T must derive from 'base'");
            assert_type<ToT>(p);
            return static_cast<const ToT*>(p);
        }

        template <typename ToT>
        static ToT* cast(ptr p)
        {
            static_assert(std::is_base_of<base, ToT>::value,
                          "T must derive from 'objects::base'");
            assert_type<ToT>(p);
            return static_cast<ToT*>(p);
        }

        template <typename ToT>
        static std::unique_ptr<ToT> cast(uptr p)
        {
            static_assert(std::is_base_of<base, ToT>::value,
                          "T must derive from 'base'");
            assert_type<ToT>(p.get());
            return std::unique_ptr<ToT>(static_cast<ToT*>(p.release()));
        }

        template <typename ToT>
        static std::shared_ptr<ToT> cast(sptr p)
        {
            static_assert(std::is_base_of<base, ToT>::value,
                          "T must derive from 'base'");
            assert_type<ToT>(p.get());
            return std::shared_ptr<ToT>(p, static_cast<ToT*>(p.get()));
        }

        std::uintptr_t type_id() const
        {
            return info_->id();
        }

        info::holder type_info() const
        {
            return info_;
        }

        virtual const char* type_name() const = 0;
        virtual uptr clone() const = 0;

    protected:
        template <typename T>
        constexpr static bool is_base()
        {
            return info::create<T>()->id() == info::create<base>()->id();
        }

        template <typename T>
        static void assert_type(base::cptr p)
        {
            if (!is_base<T>()
                && (p->type_info()->id() != info::create<T>()->id())) {
                throw std::runtime_error("object is nor base neither T");
            }
        }

    private:
        const info::holder info_;
    };

    inline bool operator<(const base::info::holder& lh,
                          const base::info::holder& rh)
    {
        return lh.type_info()->id() < rh.type_info()->id();
    }
}}}
