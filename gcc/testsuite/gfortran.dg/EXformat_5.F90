! { dg-do run }
! PR93727 Test hex float for list-directed READ.

program main
  implicit none
  call test04
  call test08
  call test10
  call test16

contains

subroutine test04
  real(4) :: r4, a, b
  real(4) :: arr(3)
  complex(4) :: z
  character(len=64) :: s

  ! Round-trip
  s = ' '
  write(s,'(EX0.0)') -huge(1.0_4)
  read(s,*) r4
  if (r4 /= -huge(1.0_4)) stop 1

  s = '0X1.8P+0'                ! = 1.5
  read(s,*) r4
  if (r4 /= 1.5_4) stop 2

  s = '-0X1.P+0'                ! = -1.0
  read(s,*) r4
  if (r4 /= -1.0_4) stop 3

  ! Lowercase
  s = '0x1.8p-1'                ! = 0.75
  read(s,*) r4
  if (r4 /= 0.75_4) stop 4

  ! Hex digit 'E' is not an exponent.
  s = '0X1.EP+3'                ! (1 + 14/16) * 8 = 15.0
  read(s,*) r4
  if (r4 /= 15.0_4) stop 5

  ! Comma-separated list of hex values.
  s = '0X1P0, 0X1.8P+1'
  read(s,*) a, b
  if (a /= 1.0_4 .or. b /= 3.0_4) stop 6

  ! Using a repeat count
  s = '3*0X1P+1'
  read(s,*) arr
  if (any(arr /= 2.0_4)) stop 7

  ! Complex constant with hex real and imaginary parts.
  s = '(0X1P0, 0X1.8P+1)'
  read(s,*) z
  if (z /= (1.0_4, 3.0_4)) stop 8
end subroutine test04


subroutine test08
  real(8) :: r8, nx
  character(len=64) :: s
  namelist /nl8/ nx

  ! Round-trip.
  s = ' '
  write(s,'(EX0.0)') -huge(1.0_8)
  read(s,*) r8
  if (r8 /= -huge(1.0_8)) stop 10

  ! Explicit hex: IEEE 754 representation of 1/3.
  s = '0X1.5555555555555P-2'
  read(s,*) r8
  if (r8 /= 1.0_8 / 3.0_8) stop 11

  ! pi, double precision.
  s = '0X1.921FB54442D18P+1'
  read(s,*) r8
  if (abs(r8 - acos(-1.0_8)) > 2.0_8 * epsilon(r8)) stop 12

  ! Hex value inside a NAMELIST.
  nx = -999.0_8
  s = '&nl8 nx=0X1.8P+3 /'
  read(s,nml=nl8)
  if (nx /= 12.0_8) stop 13

end subroutine test08


#ifdef __GFC_REAL_10__
subroutine test10
  real(10) :: r10
  character(len=64) :: s

  s = ' '
  write(s,'(EX0.0)') -huge(1.0_10)
  read(s,*) r10
  if (r10 /= -huge(1.0_10)) stop 14

  s = '0X1.8P+0'
  read(s,*) r10
  if (r10 /= 1.5_10) stop 15

end subroutine test10
#else
subroutine test10
end subroutine test10
#endif


#ifdef __GFC_REAL_16__
subroutine test16
  real(16) :: r16
  character(len=64) :: s

  s = ' '
  write(s,'(EX0.0)') -huge(1.0_16)
  read(s,*) r16
  if (r16 /= -huge(1.0_16)) stop 16

  s = '0X1.8P+0'
  read(s,*) r16
  if (r16 /= 1.5_16) stop 17

end subroutine test16
#else
subroutine test16
end subroutine test16
#endif

end program main
