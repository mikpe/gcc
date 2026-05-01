-- { dg-do link }
-- { dg-options "-fPIE -largs -pie" { target pie } }

with Ada.Text_IO; use Ada.Text_IO;

procedure PIE1 is
begin
  Put_Line ("Hello PIE World!");
end;
