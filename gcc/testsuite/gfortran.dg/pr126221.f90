! { dg-do compile }
! { dg-options "-O3" }
! { dg-additional-options "-mavx10.1 --param=vect-partial-vector-usage=2" { target { i?86-*-* x86_64-*-* } } }
!
! PR middle-end/126221

integer A(5,5)
if (any (A(:,5) /= [9, 9, 9, 9, 5])) STOP
end
