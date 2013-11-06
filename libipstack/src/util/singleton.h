#pragma once

template <typename DerivedClass>
class Singleton
{
public:
  static DerivedClass & Inst()
  {
    static DerivedClass instance;
    return instance;
  }
};