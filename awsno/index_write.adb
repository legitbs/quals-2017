with AWS.Parameters;
with AWS.Net;
With Ada.Streams;

package body index_write is
  use AWS;

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

  function main (http_request : AWS.Status.Data) return AWS.Response.Data is
      web_socket  : AWS.Net.Socket_Type'Class := AWS.Status.Socket(http_request);
      data        : String           := (1..1024 => character'val(0));
      dlen        : Integer          := 0;
      index       : Integer;
      instring    : String(1..512);
  begin

    loop
      send_line( web_socket, "[+] Enter Index: ", 17);
      read_line( web_socket, data, dlen);

      index := Integer'value( data(1..dlen-1) );

      exit when index = 0;

      send_line( web_socket, "[+] Enter Value: ", 17);
      read_line( web_socket, data, dlen);

      instring(index) := data(1);

      send_line( web_socket, "[+] Current: ", 13);
      send_line( web_socket, instring, 512);
      send_line( web_socket, "" & character'val(10), 1);
    end loop;

    return AWS.Response.Build
        ("text/html", "<html><body>"
           & "<p>Index Write Over<br>"
           & "</body></html>");

  end main;

end index_write;