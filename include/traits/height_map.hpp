#ifndef HEIGHT_MAP_H_
#define HEIGHT_MAP_H_

#include <type_traits>

// going with git in submodule for now
// if i get screwed over when it comes time to negotiate dependencies i'll figure it out (it would happen anyway i think)

namespace terraingen {
  namespace traits {
    namespace impl_ {
      struct height_map_impl {
        template <typename MapType,
        typename Sample = std::is_same<float, decltype(std::declval<MapType&>().Get((int)0, (int)0))>>
        static std::true_type test(int);

        template <typename MapType, typename...>
        static std::false_type test(...);
      };
    }

    template <typename T>
    struct height_map : decltype(impl_::height_map_impl<T>(0)) {};
  }
}

#endif // HEIGHT_MAP_H_