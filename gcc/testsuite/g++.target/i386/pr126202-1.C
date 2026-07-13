/* { dg-do compile { target pie } } */
/* { dg-options "-O3 -std=gnu++20 -march=znver5 -fPIE" } */

typedef int fixed16_16;
struct vec2_fixed
{
  fixed16_16 x, z;
} s_polygonUv[32];
struct vec3_fixed
{
  fixed16_16 x, y, z;
};
template <typename> struct allocator_traits;
template <typename> class allocator;
template <typename _Tp> struct allocator_traits<allocator<_Tp>>
{
  using value_type = _Tp;
  using pointer = _Tp *;
  template <typename _Up> using rebind_alloc = allocator<_Up>;
};
template <typename _Alloc> struct __alloc_traits : allocator_traits<_Alloc>
{
  typedef allocator_traits<_Alloc> _Base_type;
  typedef _Base_type::value_type reference;
  template <typename _Tp> struct rebind
  {
    typedef typename _Base_type::rebind_alloc<_Tp> other;
  };
};
template <typename _Tp, typename _Alloc> struct _Vector_base
{
  typedef __alloc_traits<_Alloc>::template rebind<_Tp>::other _Tp_alloc_type;
  struct
  {
    __alloc_traits<_Tp_alloc_type>::pointer _M_start;
  } _M_impl;
};
template <typename _Tp, typename _Alloc = allocator<_Tp>>
struct vector : _Vector_base<_Tp, _Alloc>
{
  __alloc_traits<typename _Vector_base<_Tp, _Alloc>::_Tp_alloc_type>::reference
  operator[] (int __n)
  {
    return *(this->_M_impl._M_start + __n);
  }
};
extern int vertexCount, s_polygonIntensity_0;
struct JmPolygon
{
  int *indices;
};
extern vector<vec3_fixed> s_verticesVS;
extern vector<fixed16_16> s_vertexIntensity;
extern vec3_fixed s_polygonVerticesVS[2];
extern JmPolygon *robj3d_setupPolygon_polygon;
void
robj3d_setupPolygon ()
{
  for (int v = 0; v < vertexCount; v++)
    s_polygonVerticesVS[v]
        = s_verticesVS[robj3d_setupPolygon_polygon->indices[v]];
  vec2_fixed *uv = (vec2_fixed *)robj3d_setupPolygon_polygon;
  vec2_fixed zero{};
  for (int v = 0; v < vertexCount; v++)
    s_polygonUv[v] = uv[v];
  for (int v = 0; v < vertexCount; v++)
    s_polygonUv[v] = zero;
  for (int v;;)
    s_polygonIntensity_0
        = s_vertexIntensity[robj3d_setupPolygon_polygon->indices[v]];
}
