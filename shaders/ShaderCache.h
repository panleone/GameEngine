#ifndef SHADER_CACHE_C
#define SHADER_CACHE_C

#include <functional>
#include <unordered_map>

template <typename K, typename T> class ShaderCache {
private:
  std::unordered_map<K, T> cache;
  std::function<T(K &)> keyValMap;

public:
  ShaderCache(std::function<T(K &)> keyValMap)
      : keyValMap{std::move(keyValMap)} {};
  const T &get(K &key) {
    if (cache.count(key) == 0) {
      cache.insert(std::pair(key, keyValMap(key)));
    }
    return cache.at(key);
  }
};

#endif // SHADER_CACHE_C