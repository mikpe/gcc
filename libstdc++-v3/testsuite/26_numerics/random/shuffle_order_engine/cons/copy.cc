// { dg-do run { target c++11 } }
// { dg-require-cstdint "" }
//
// 2010-02-16  Paolo Carlini  <paolo.carlini@oracle.com>
//
// Copyright (C) 2010-2025 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include <random>

void
test01()
{
  std::shuffle_order_engine
    <
      std::linear_congruential_engine<uint_fast32_t,16807UL, 0UL, 2147483647UL>,
      256
    > e(1);

  const auto f(e);
  auto g(f);
  g = g; // Suppress unused warning.
}

int main()
{
  test01();
  return 0;
}
