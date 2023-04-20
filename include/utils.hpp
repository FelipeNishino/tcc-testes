#ifndef TCC_UTILS_H
#define TCC_UTILS_H

#include <string>
#include <array>

static const std::array<std::string, 4> EMO_TO_STR = {"happy", "sad", "angry", "relaxed"};
namespace detail {
    struct advance {
        template <typename T> void operator()(T& t) const { ++t; }
    };

    // Adaptation of for_each_arg, see:
    // https://isocpp.org/blog/2015/01/for-each-argument-sean-parent
    template <class... Iterators>
    void advance_all(Iterators&... iterators) {
        [](...){}((advance{}(iterators), 0)...);
    }
} // namespace detail

template <typename F, typename Iterator, typename ... ExtraIterators>
F for_each_zipped(
    F func, 
    Iterator begin, 
    Iterator end, 
    ExtraIterators ... extra_iterators)
{
    for(;begin != end; ++begin, detail::advance_all(extra_iterators...))
        func(*begin, *(extra_iterators)... );
    return func;
}
template <typename F, typename Container, typename... ExtraContainers>
F for_each_zipped_containers(
    F func,
    Container& container, 
    ExtraContainers& ... extra_containers)
{
    return for_each_zipped(
        func, std::begin(container), std::end(container), std::begin(extra_containers)...);
}

#endif //TCC_UTILS_H