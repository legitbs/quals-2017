with AWS.Parameters;
with AWS.Net;
with Ada.Streams;
with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;
with trains_h; use trains_h;
with Ada.Strings.Unbounded; use Ada.Strings.Unbounded;

with Ada.Text_IO; use Ada.Text_IO;
with AWS.Parameters;

package body trains is
  package train_ll is new Ada.Containers.Doubly_Linked_Lists(train_data);

  train_list   : train_ll.List;
  list_len     : Integer := 0;

  procedure add_train(fd : Interfaces.C.int ) is
    retval : Interfaces.C.int := 0;
    ss     : String           := (1..32 => character'val(0));
    ocp    : Interfaces.C.Strings.chars_ptr := Interfaces.C.Strings.New_String(ss);
    nt     : train_data;
  begin
    retval := send_string( fd, Interfaces.C.Strings.New_string("Enter the Name: ") );
    retval := read_line( fd, ocp, 31);

    if retval <= 0 then
      return;
    end if;

    nt.name := to_unbounded_string( Interfaces.C.Strings.Value( ocp ));

    retval := send_string( fd, Interfaces.C.Strings.New_string("Enter the Model: ") );
    retval := read_line( fd, ocp, 31);

    if retval <= 0 then
      return;
    end if;

    nt.model := to_unbounded_string( Interfaces.C.Strings.Value( ocp ));

    retval := send_string( fd, Interfaces.C.Strings.New_string("Enter the Type: ") );
    retval := read_line( fd, ocp, 31);

    if retval <= 0 then
      return;
    end if;

    nt.t := to_unbounded_string( Interfaces.C.Strings.Value( ocp ));

    retval := send_string( fd, Interfaces.C.Strings.New_string("Enter the Max Speed: ") );
    retval := read_line( fd, ocp, 31);

    if retval <= 0 then
      return;
    end if;

    nt.speed := Float'value( Interfaces.C.Strings.Value( ocp ));

    retval := send_string( fd, Interfaces.C.Strings.New_string("Enter the Max Passengers: ") );
    retval := read_line( fd, ocp, 31);

    if retval <= 0 then
      return;
    end if;

    nt.passengers := Integer'value( Interfaces.C.Strings.Value( ocp ));

    train_list.append(nt);
    list_len := list_len + 1;

    return;
  end add_train;

  procedure update_train( fd : Interfaces.C.int ) is
    ts     : String := (1 => character'val(0) );
    tslen  : integer      := 0;
    tt     : train_data;
    choice : integer      := 1;
    ps     : train_ll.Cursor;
    tc     : train_ll.Cursor;
    retval : Interfaces.C.Int;
    ss     : String           := (1..32 => character'val(0));
    ocp    : Interfaces.C.Strings.chars_ptr := Interfaces.C.Strings.New_String(ss);
  begin
    retval := send_string( fd, Interfaces.c.strings.new_string("Would you like to print the available trains (y/n): ") );
    retval := read_line( fd, ocp, 31);

    if retval <= 0 then
      return;
    end if;

    ts := Interfaces.C.Strings.Value( ocp )(1..1);

    if ts(1) = 'y' then
      for index in train_list.iterate loop
        tt := train_ll.Element( index );

        retval := send_string( fd, Interfaces.c.strings.new_string(Integer'image(choice) & " ) " & to_string(tt.name) & " " & to_string(tt.model) & character'val(10)) );

        choice := choice + 1;
      end loop;

    end if;

    retval := send_string( fd, Interfaces.c.strings.new_string("Enter the index: ") );
    choice := readint( fd );

    if choice > list_len or choice <= 0 then
      retval := send_string( fd, Interfaces.c.strings.new_string("Invalid choice" & character'val(10)) );
      return;
    end if;

    tslen := 1;

    tc := train_ll.first( train_list );

    for index in train_list.iterate loop
      if tslen = choice then
        ps := index;
        exit;
      end if;

      tslen := tslen + 1;
    end loop;

    tt := train_ll.element( ps );

    train_ll.delete( train_list, ps);

    retval := send_string( fd, Interfaces.c.strings.new_string("Update Name(y/n): ") );
    retval := read_line( fd, ocp, 2);

    if retval <= 0 then
      return;
    end if;

    ts := Interfaces.C.Strings.Value( ocp )(1..1);

    if ts(1) = 'y' then
      retval := send_string( fd, Interfaces.c.strings.new_string(": ") );
      retval := read_line( fd, ocp, 31);
      tt.name := to_unbounded_string( Interfaces.C.Strings.Value( ocp ) );
    end if;

    train_list.append(tt);

    return;
  end update_train;

  procedure list_trains ( fd : Interfaces.C.int ) is
    tt : train_data;
    retval : Interfaces.C.int;
  begin
    for index in train_list.iterate loop
      tt := train_ll.Element( index );

      retval := send_string( fd, Interfaces.C.Strings.New_string("Name: " & to_string(tt.name) & character'val(10)) );
      retval := send_string( fd, Interfaces.C.Strings.New_string("Model: " & to_string(tt.model) & character'val(10)) );
      retval := send_string( fd, Interfaces.C.Strings.New_string("Type: " & to_string(tt.t) & character'val(10)) );
      retval := send_string( fd, Interfaces.C.Strings.New_string("Speed: " & Float'image(tt.speed) & character'val(10)) );
      retval := send_string( fd, Interfaces.C.Strings.New_string("Passengers: " & Integer'image(tt.passengers) & character'val(10) & character'val(10)) );

    end loop;

    return;
  end list_trains;

  procedure index_write_train_name( fd : Interfaces.C.int ) is
    retval : Interfaces.C.int;
    ss     : String           := (1..32 => character'val(0));
    ts     : unbounded_string;
    ocp    : Interfaces.C.Strings.chars_ptr := Interfaces.C.Strings.New_String(ss);
    index  : integer;
  begin

    loop
      retval := send_string( fd, Interfaces.c.strings.new_string("[+] Enter Index: "));
      retval := read_line( fd, ocp, 450);

      index := Integer'value( Interfaces.C.Strings.Value( ocp ) );

      exit when index = 0;

    end loop;
  end index_write_train_name;
  
  procedure delete_train( fd : Interfaces.C.int ) is
    ts     : String := (1..10 => character'val(0) );
    tslen  : Integer     := 0;
    choice : Integer     := 1;
    tt     : train_data;
    tc     : train_ll.Cursor;
    retval : Interfaces.C.Int;
    ss     : String           := (1..1024 => character'val(0));
    ocp    : Interfaces.C.Strings.chars_ptr := Interfaces.C.Strings.New_String(ss);

  begin
    retval := send_string( fd, Interfaces.c.strings.new_string("Would you like to print the available planes (y/n): ") );
    retval := read_line( fd, ocp, 2);

    if retval <= 0 then
      return;
    end if;

    ts := Interfaces.C.Strings.Value( ocp );

    if ts(1) = 'y' then
      for index in train_list.iterate loop
        tt := train_ll.Element( index );

        retval := send_string( fd, Interfaces.c.strings.new_string(Integer'image(choice) & " ) " & to_string(tt.name) & " " & to_string(tt.model) & character'val(10)) );

        choice := choice + 1;
      end loop;

    end if;

    retval := send_string( fd, Interfaces.c.strings.new_string("Enter the index: ") );
    choice := readint( fd );

    if choice > list_len or choice <= 0 then
      retval := send_string( fd, Interfaces.c.strings.new_string("Invalid choice" & character'val(10)) );
      return;
    end if;

    tslen := 1;

    tc := train_ll.first( train_list );

    while tslen < choice loop
      train_ll.next( tc );
      tslen := tslen + 1;
    end loop;

    train_ll.delete( train_list, tc);

    return;
  end delete_train;

  function main (http_request : AWS.Status.Data) return AWS.Response.Data is
    socket : AWS.Net.Socket_Type'Class := AWS.Status.Socket(http_request);
    fd     : Interfaces.C.int := Interfaces.C.int(AWS.Net.Get_FD( socket ));
    retval : integer := 0;
  begin

    while retval /= 5 loop
      menu( fd );
      retval := readint( fd );

      if retval = 1 then
        add_train( fd );
      elsif retval = 2 then
        list_trains(fd);
      elsif retval = 3 then
        delete_train(fd);
      elsif retval = 4 then
        update_train(fd);
      elsif retval = 7 then
        index_write_train_name(fd);
      end if;

    end loop;

    return AWS.Response.Build("text/html", "<html><body><p>Thanks for the dataz <br></body></html>");
  end main;

end trains;