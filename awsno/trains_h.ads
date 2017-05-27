pragma Ada_2005;
pragma Style_Checks (Off);

with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;

package trains_h is

   function send_string (arg1 : int; arg2 : Interfaces.C.Strings.chars_ptr) return int;  -- trains.h:4
   pragma Import (C, send_string, "send_string");

   procedure menu (arg1 : int);  -- trains.h:6
   pragma Import (C, menu, "menu");

   function readint (arg1 : int) return integer;  -- trains.h:8
   pragma Import (C, readint, "readint");

   function read_line
     (arg1 : int;
      arg2 : Interfaces.C.Strings.chars_ptr;
      arg3 : int) return int;  -- trains.h:10
   pragma Import (C, read_line, "read_line");

end trains_h;
