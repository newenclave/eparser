#pragma once
#include <algorithm>
#include <string>

namespace eparser { namespace common { namespace helpers {

    struct strings {

        template <typename CharT, typename ItrT>
        static std::basic_string<CharT> to_string(ItrT begin, ItrT end)
        {
            std::basic_string<CharT> result;
            for (; (begin != end) && (*begin != static_cast<CharT>(0));
                 ++begin) {
                result.push_back(static_cast<CharT>(*begin));
            }
            return result;
        }

        template <typename CharT, typename ContT>
        static std::basic_string<CharT> to_string(const ContT& container)
        {
            return to_string<CharT>(std::begin(container), std::end(container));
        }
    };

    struct reader {

        static bool is_digit(char c)
        {
            return ('0' <= c && c <= '9');
        }

        static bool is_digit_(char c)
        {
            return is_digit(c) || c == '_';
        }

        template <typename Itr>
        static bool is_ident(Itr begin, Itr end)
        {
            return std::find_if_not(
                       begin, end,
                       [](auto c) { return is_ident(static_cast<char>(c)); })
                == end;
        }

        static bool is_ident(char c)
        {
            return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')
                || is_digit_(c);
        }

        template <typename CharT>
        static bool is_space(CharT c)
        {
            switch (c) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                return true;
            }
            return false;
        }

        template <typename CharT>
        static bool is_gap(CharT c)
        {
            return c == '_';
        }

        template <typename Itr>
        static Itr skip_spaces(Itr begin, Itr end)
        {
            return std::find_if_not(begin, end,
                                    [](auto c) { return is_space(c); });
        }

        template <typename Itr>
        static Itr read_ident(Itr begin, Itr end)
        {
            return std::find_if_not(begin, end,
                                    [](auto c) { return is_ident(c); });
        }

        template <typename Itr>
        static Itr read_number(Itr begin, Itr end)
        {
            return std::find_if_not(begin, end,
                                    [](auto c) { return is_digit_(c); });
        }

        template <typename Itr, typename Itr2>
        static bool begins_with(Itr bTest, Itr eTest, Itr2 bVal, Itr2 eVal)
        {
            for (; (bVal != eVal) && (bTest != eTest) && (*bTest == *bVal);
                 ++bVal, ++bTest) {
            }
            return bVal == eVal;
        }

        template <typename Itr, typename ContaiterT>
        static bool begins_with(Itr bTest, Itr eTest,
                                const ContaiterT& container)
        {
            return begins_with(bTest, eTest, std::begin(container),
                               std::end(container));
        }

        template <typename ItrT, typename ContainerT>
        static std::basic_string<typename ItrT::value_type>
        read_string(ItrT& begin, ItrT end, const ContainerT& stop)
        {
            using string_type = std::basic_string<typename ItrT::value_type>;
            string_type res;
            for (; (begin != end) && !begins_with(begin, end, stop); ++begin) {

                auto next = std::next(begin);

                if (*begin == '\\' && next != end) {
                    if (stop[0] == *next) {
                        res.push_back(stop[0]);
                    } else {
                        switch (*next) {
                        case 'n':
                            res.push_back('\n');
                            break;
                        case 'r':
                            res.push_back('\r');
                            break;
                        case 't':
                            res.push_back('\t');
                            break;
                        case '\\':
                            res.push_back('\\');
                            break;
                        case '"':
                            res.push_back('"');
                            break;
                        case '\'':
                            res.push_back('\'');
                            break;
                        case '0':
                            res.push_back('\0');
                            break;
                        default:
                            res.push_back('\\');
                            res.push_back(*next);
                            break;
                        }
                    }
                    begin = next;
                    if (begin == end) {
                        break;
                    }
                } else {
                    res.push_back(*begin);
                }
            }
            if (begin != end) {
                ++begin;
            }
            return res;
        }

        template <typename CharT>
        static bool valid_for_dec(CharT c)
        {
            return ('0' <= c) && (c <= '9');
        }

        template <typename CharT>
        static bool valid_for_dec_(CharT c)
        {
            return valid_for_dec(c) || is_gap(c);
        }

        template <typename ItrT>
        static bool check_if_float(ItrT begin, ItrT end)
        {
            bool float_found = false;
            while (begin != end && !float_found) {
                char c = *begin++;
                if ((c == 'e') || (c == 'E')) {
                    float_found = true;
                } else if (c == '.') {
                    if ((begin != end) && (*begin == '.')) {
                        return false;
                    } else {
                        float_found = true;
                    }
                } else if (!valid_for_dec_(c)) {
                    return false;
                }
            }
            return float_found;
        }

        template <typename ContT>
        static double read_float(const ContT& cont)
        {
            auto bb = std::begin(cont);
            return read_float(bb, std::end(cont));
        }

        template <typename ItrT>
        static ItrT read_float(ItrT s, ItrT end)
        {
            return read_float(s, end, nullptr);
        }

        template <typename ItrT>
        static ItrT read_float(ItrT s, ItrT end, double* result)
        {
            double a = 0.0;
            int e = 0;
            int c = 0;

            while ((s != end) && valid_for_dec_(c = *s++)) {
                if (!is_gap(c)) {
                    a = a * 10.0 + (c - '0');
                }
            }

            if (c == '.') {
                while ((s != end) && valid_for_dec_(c = *s++)) {
                    if (!is_gap(c)) {
                        a = a * 10.0 + (c - '0');
                        e = e - 1;
                    }
                }
            }

            if (c == 'e' || c == 'E') {
                int sign = 1;
                int i = 0;
                c = *s++;

                if (c == '+') {
                    c = *s++;
                } else if (c == '-') {
                    c = *s++;
                    sign = -1;
                }

                while (valid_for_dec_(c)) {
                    if (is_gap(c)) {
                        c = *s++;
                    }
                    i = i * 10 + (c - '0');
                    if (s == end) {
                        break;
                    } else {
                        c = *s++;
                    }
                }
                e += i * sign;
            }

            while (e > 0) {
                a *= 10.0;
                e--;
            }

            while (e < 0) {
                a *= 0.1;
                e++;
            }

            if (s != end) {
                --s;
            }
            if (result) {
                *result = a;
            }
            return s;
        }

        template <typename CharT>
        static std::uint8_t char2int(CharT cc)
        {
            std::uint8_t c = static_cast<std::uint8_t>(cc);
            switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return c - '0';
                // case 'a': case 'b': case 'c':
                // case 'd': case 'e': case 'f':
                //	return c - 'a' + 0xa;
                // case 'A': case 'B': case 'C':
                // case 'D': case 'E': case 'F':
                //	return c - 'A' + 0xA;
            }
            return 0xFF;
        }

        template <typename ItrT>
        static std::int64_t read_int(ItrT begin, ItrT end,
                                     int* first_inval = nullptr)
        {
            std::uint64_t res = 0;
            *first_inval = -1;

            int pos = 0;
            for (; begin != end; ++begin) {
                auto c = *begin;
                if (!is_gap(c)) {
                    if (valid_for_dec_(c)) {
                        res *= 10;
                        res += char2int(c);
                        ++pos;
                    } else {
                        if (first_inval) {
                            *first_inval = pos;
                        }
                        return 0;
                    }
                }
            }
            return res;
        }
    };

}}}