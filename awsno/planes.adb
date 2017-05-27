with AWS.Parameters;
with AWS.Net;
with Ada.Streams;

with Ada.Text_IO; use Ada.Text_IO;
with AWS.Parameters;

package body planes is

  package plane_ll is new Ada.Containers.Doubly_Linked_Lists(plane_data);

  plane_list   : plane_ll.List;
  list_len     : Integer := 0;

  package body text_io is
    function to_string( in_stream : Ada.Streams.Stream_Element_Array; 
          stream_len : Ada.Streams.Stream_Element_Offset ) return String is
          out_string : String := ( 1 .. 1024 => Character'Val(0) );
    begin
          for index in Integer range 1 .. Integer'Val(stream_len) loop
              out_string(index) := Character'Val(  in_stream( Ada.Streams.Stream_Element_Offset(index) )  );
          end loop;

          return out_string;
    end to_string;

    function to_stream ( in_string : String; string_len : Integer ) return Ada.Streams.Stream_Element_Array is
      out_stream : Ada.Streams.Stream_Element_Array (1 .. Ada.Streams.Stream_Element_Offset (string_len));
    begin

        for index in Integer range 1 .. string_len loop
          out_stream( Ada.Streams.Stream_Element_Offset (index) ) := Ada.Streams.Stream_Element( Character'Pos(in_string(index)));
        end loop;

        return out_stream;
    end to_stream;

    procedure read_line( socket : in AWS.Net.Socket_Type'Class; in_string : out String ; outlen : out Integer ) is
          in_data : Ada.Streams.Stream_Element_array(1..1024);
          in_len : Ada.Streams.Stream_Element_Offset;
    begin
          AWS.Net.Receive( socket, in_data, in_len);

          in_string := to_string( in_data, in_len );

          outlen := Integer'Val( in_len );
    end read_line;

    procedure send_line( socket : AWS.Net.Socket_Type'Class; out_data : String; out_len : Integer ) is
          out_stream : Ada.Streams.Stream_Element_array(1..Ada.Streams.Stream_Element_Offset(out_len)) := to_stream(out_data, out_len);
          out_stream_len : Ada.Streams.Stream_Element_Offset := Ada.Streams.Stream_Element_Offset(out_len);
    begin

          AWS.Net.Send( socket, out_stream(1..out_stream_len));

          return;
    end send_line;

    function Ada_String_To_Integer ( in_string : String ) return Integer is
    i : Integer;
    c : character;
    e : Integer := in_string'length;
    begin 

      for x in integer range 1 .. in_string'length loop
        c := in_string(x);

        i := character'pos(c);

        if i = 10 or i = 0 then
          e := x - 1;
          exit;
        end if;
      end loop;

      i := integer'value(in_string(in_string'First..e));

      return i;
    end Ada_String_To_Integer;

    procedure Get_Ada_String( socket : in AWS.Net.Socket_Type'Class; in_string : out String ; outlen : out Integer ) is
      id   :  Integer  := 1;
      in_data : Ada.Streams.Stream_Element_array(1..1024);
      in_len : Ada.Streams.Stream_Element_Offset;
      i    : Integer;
    begin
      AWS.Net.Receive( socket, in_data, in_len);

      while id <= integer'val(in_len) loop
        in_string(id) := character'val(in_data(Ada.Streams.Stream_Element_Offset(id)));

        id := id + 1;

        i := Ada_String_To_Integer( in_string );
      end loop;

      outlen := id;
    end Get_Ada_String;

    procedure T579bX( socket : in AWS.Net.Socket_Type'Class; i : out Integer ) is
        ts    : String := (1..256 => character'val(0));
        tslen : Integer;
    begin
        Get_Ada_String( socket, ts, tslen);

        i := Ada_String_To_Integer( ts );

    end T579bX;
  end text_io;

  procedure menu( socket : in AWS.Net.Socket_Type'Class ) is
    option_one : String := "1) Add Plane" & Character'Val(10);
    option_two : String := "2) List Planes" & Character'Val(10);
    option_thr : String := "3) Remove Plane" & Character'Val(10);
    option_fou : String := "5) Exit" & Character'Val(10);
    prompt : String := "> ";
  begin
    text_io.send_line( socket, option_one, option_one'Length );
    text_io.send_line( socket, option_two, option_two'Length );
    text_io.send_line( socket, option_thr, option_thr'Length );
    text_io.send_line( socket, option_fou, option_fou'Length );
    text_io.send_line( socket, "4) Update Plane" & character'val(10), 16);
    text_io.send_line( socket, prompt, prompt'Length );
  end menu;

  function read_selection( socket : in AWS.Net.Socket_Type'Class ) return Integer is
    in_string : String(1..1024);
    data_len  : Integer         := 0;
    i         : Integer         := 0;
  begin

    text_io.read_line( socket, in_string, data_len );

    i := Integer'value( in_string(1..(data_len-1)) );

    return i;
  end read_selection;

  procedure print_list ( socket : in AWS.Net.Socket_Type'Class ) is
    tp : plane_data;
  begin
    for index in plane_list.iterate loop
      tp := plane_ll.Element( index );

      text_io.send_line( socket, "Manufacturer: ", 14);
      text_io.send_line( socket, to_string(tp.manufacture), to_string(tp.manufacture)'Length);
      text_io.send_line( socket, "" & character'val(10), 1);

      text_io.send_line( socket, "Cost: ", 6);
      text_io.send_line( socket, Float'Image(tp.cost), Float'Image(tp.cost)'Length);
      text_io.send_line( socket, "" & character'val(10), 1);

      text_io.send_line( socket, "Model: ", 7);
      text_io.send_line( socket, to_string(tp.model), to_string(tp.model)'Length);
      text_io.send_line( socket, "" & character'val(10), 1);

      text_io.send_line( socket, "Passengers: ", 12);
      text_io.send_line( socket, Integer'Image(tp.passengers), Integer'Image(tp.passengers)'Length);
      text_io.send_line( socket, "" & character'val(10), 1);

      text_io.send_line( socket, "MPH: ", 4);
      text_io.send_line( socket, Float'Image(tp.mph), Float'Image(tp.mph)'Length);
      text_io.send_line( socket, "" & character'val(10) & character'val(10), 2);
      
    end loop;

    return;
  end print_list;

  procedure add_plane( socket : in AWS.Net.Socket_Type'Class ) is
    ts    : String        := (1..1024 => character'val(0) );
    tslen : Integer       := 0;
    np    : plane_data;
  begin
    text_io.send_line( socket, "Enter the Manufacturer: ", 24);
    text_io.read_line( socket, ts, tslen );

    np.manufacture := to_unbounded_string( ts(1..tslen-1) );

    text_io.send_line( socket, "Enter the Passenger Count: ", 27);
    text_io.read_line( socket, ts, tslen );

    np.passengers := Integer'value( ts(1..tslen-1) );

    text_io.send_line( socket, "Enter the Type: ", 16);
    text_io.read_line( socket, ts, tslen );

    np.model := to_unbounded_string( ts(1..tslen-1) );

    text_io.send_line( socket, "Enter the Cost: ", 16);
    text_io.read_line( socket, ts, tslen );

    np.cost := Float'value( ts(1..tslen-1));

    text_io.send_line( socket, "Enter the MPH: ", 15);
    text_io.read_line( socket, ts, tslen );

    np.mph := Float'value( ts(1..tslen-1));

    plane_list.append( np );
    list_len := list_len + 1;

    return;
  end add_plane;

  procedure delete_plane( socket : in AWS.Net.Socket_Type'Class ) is
    ts     : String      := (1..1024 => character'val(0));
    tslen  : Integer     := 0;
    choice : Integer     := 1;
    tp     : plane_data;
    ps     : plane_ll.Cursor;

  begin
    text_io.send_line( socket, "Would you like to print the available planes (y/n): ", 52);
    text_io.read_line( socket, ts, tslen);

    if ts(1) = 'y' then
      for index in plane_list.iterate loop
        tp := plane_ll.Element( index );

        text_io.send_line( socket, Integer'image(choice), Integer'image(choice)'length);
        text_io.send_line( socket, ") ", 2);
        text_io.send_line( socket, to_string(tp.manufacture), to_string(tp.manufacture)'Length);
        text_io.send_line( socket, " ", 1);
        text_io.send_line( socket, to_string(tp.model), to_string(tp.model)'Length);
        text_io.send_line( socket, "" & character'val(10), 1);

        choice := choice + 1;
      end loop;

    end if;

    text_io.send_line( socket, "Enter the index: ", 17);
    choice := read_selection( socket );

    if choice > list_len or choice <= 0 then
      text_io.send_line( socket, "Invalid choice" & character'val(10), 15);
      return;
    end if;

    tslen := 1;

    ps := plane_ll.first( plane_list );

    while tslen < choice loop
      plane_ll.next( ps );
      tslen := tslen + 1;
    end loop;

    plane_ll.delete( plane_list, ps);

    return;
  end delete_plane;

  procedure update_plane( socket : in AWS.Net.Socket_Type'Class ) is
    ts    : string       := (1..1024 => character'val(0));
    tslen : integer      := 0;
    tp    : plane_data;
    choice: integer      := 0;
    ps    : plane_ll.Cursor;
  begin
    text_io.send_line( socket, "Would you like to print the available planes (y/n): ", 52);
    text_io.read_line( socket, ts, tslen);

    if ts(1) = 'y' then
      for index in plane_list.iterate loop
        tp := plane_ll.Element( index );

        text_io.send_line( socket, Integer'image(choice), Integer'image(choice)'length);
        text_io.send_line( socket, ") ", 2);
        text_io.send_line( socket, to_string(tp.manufacture), to_string(tp.manufacture)'Length);
        text_io.send_line( socket, " ", 1);
        text_io.send_line( socket, to_string(tp.model), to_string(tp.model)'Length);
        text_io.send_line( socket, "" & character'val(10), 1);

        choice := choice + 1;
      end loop;
    end if;

    text_io.send_line( socket, "Enter the index: ", 17);
    choice := read_selection( socket );

    if choice > list_len or choice <= 0 then
      text_io.send_line( socket, "Invalid choice" & character'val(10), 15);
      return;
    end if;

    tslen := 1;

    ps := plane_ll.first( plane_list );

    while tslen < choice loop
      ps := plane_ll.next( ps );
      tslen := tslen + 1;
    end loop;

    tp := plane_ll.element( ps );

    text_io.send_line( socket, "Update Manufacturer (y/n): ",  27);
    text_io.read_line( socket, ts, tslen);

    if ts(1) = 'y' then
      text_io.send_line(socket, ": ", 2);
      text_io.read_line( socket, ts, tslen);

      tp.manufacture := to_unbounded_string( ts(1..tslen-1) );
    end if;

    text_io.send_line( socket, "Update Type (y/n): ",  19);
    text_io.read_line( socket, ts, tslen);

    if ts(1) = 'y' then
      text_io.send_line(socket, ": ", 2);
      text_io.read_line( socket, ts, tslen);
      tp.model := to_unbounded_string( ts(1..tslen-1) );
    end if;

    text_io.send_line( socket, "Update Passenger Count (y/n): ",  30);
    text_io.read_line( socket, ts, tslen);

    if ts(1) = 'y' then
      text_io.send_line(socket, ": ", 2);
      text_io.read_line( socket, ts, tslen);

      tp.passengers := Integer'value(ts(1..tslen-1));
    end if;

    text_io.send_line( socket, "Update Cost (y/n): ",  19);
    text_io.read_line( socket, ts, tslen);

    if ts(1) = 'y' then
      text_io.send_line(socket, ": ", 2);
      text_io.read_line( socket, ts, tslen);
      tp.cost := float'value(ts(1..tslen-1));
    end if;

    text_io.send_line( socket, "Update MPH (y/n): ",  18);
    text_io.read_line( socket, ts, tslen);

    if ts(1) = 'y' then
      text_io.send_line(socket, ": ", 2);
      text_io.read_line( socket, ts, tslen);
      tp.mph := float'value(ts(1..tslen-1));
    end if;


    return;
  end update_plane;

  function main (http_request : AWS.Status.Data) return AWS.Response.Data is
    socket : AWS.Net.Socket_Type'Class := AWS.Status.Socket(http_request);
    choice : Integer := 0;
  begin

    while choice /= 5 loop
      menu( socket );
      text_io.T579bX( socket, choice );

      if choice = 1 then
        add_plane( socket );
      elsif choice = 2 then
        print_list( socket );
      elsif choice = 3 then
        delete_plane(socket);
      elsif choice = 4 then
        update_plane(socket);
      end if;

    end loop;

    return AWS.Response.Build("text/html", "<html><body><p>Thanks for the dataz <br></body></html>");
  end main;

end planes;