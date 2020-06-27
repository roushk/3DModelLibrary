#pragma once


//singleton instance of a type utilizing a static getter

namespace pattern
{
  template <typename T>
  class SingletonDeleter
  {
  public:
    void operator()(const T * const singletonInstance) { delete singletonInstance; }
  };

  template <typename T>
  class Singleton
  {
    //no initializer
    Singleton() {}
  public:
    //gets a static instance of a single type
    static T& get()
    {
      static std::unique_ptr<T, SingletonDeleter<T>> pointer(new T());
      return *pointer;
    }
  };

  template <typename T>
  T& get()
  {
    return Singleton<T>::get();
  }
}
