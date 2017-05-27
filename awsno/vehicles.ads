with AWS.Response;
with AWS.Status;
with Ada.Strings.Unbounded; use Ada.Strings.Unbounded;
with Ada.Containers.Doubly_Linked_Lists;
with Ada.Streams;
with AWS.Net;

package vehicles is

    package text_io is
        function to_string( in_stream : Ada.Streams.Stream_Element_Array; stream_len : Ada.Streams.Stream_Element_Offset ) return String;
        function to_stream ( in_string : String; string_len : Integer ) return Ada.Streams.Stream_Element_Array;
        procedure read_line( socket : in AWS.Net.Socket_Type'Class; in_string : out String ; outlen : out Integer );
        procedure send_line( socket : AWS.Net.Socket_Type'Class; out_data : String; out_len : Integer );
        function Ada_String_To_Integer ( in_string : String ) return Integer;
        procedure Get_Ada_String( socket : in AWS.Net.Socket_Type'Class; in_string : out String ; outlen : out Integer );
        procedure T579bX( socket : in AWS.Net.Socket_Type'Class; i : out Integer );
    end text_io;

    type vehicle_data is record
        make        : Unbounded_String;
        model       : Unbounded_String;
        cost        : Float;
        mpg         : Float;
        year        : Integer;
    end record;


   function main (http_request : AWS.Status.Data) return AWS.Response.Data;

end vehicles;