with AWS.Parameters;
with AWS.Net;
with Ada.Streams;

with Ada.Text_IO; use Ada.Text_IO;
with Interfaces.C;
with input_form_cb; use input_form_cb;
with index_write; use index_write;
with vehicles; use vehicles;
with planes; use planes;
with trains; use trains;

package body awsno_cb is
    use AWS;

   function main_server (http_request : AWS.Status.Data) return AWS.Response.Data is
      request_uri        : constant String           := AWS.Status.URI (http_request);
      request_params     : constant Parameters.List  := AWS.Status.Parameters (http_request);
      web_socket         : AWS.Net.Socket_Type'Class := AWS.Status.Socket(http_request);
    begin
        Put_Line(request_uri);

        if request_uri = "/ping" then	 

	        return AWS.Response.Build( "text/html", "<p>PONG!</p>");
        elsif request_uri = "/twitter" then
          return AWS.Response.URL (Location => "https://twitter.com/hawaii_john");
        elsif request_uri = "/form" then
          return input_form_cb.input_form( http_request );
        elsif request_uri = "/index_write" then
          return index_write.main( http_request );
        elsif request_uri = "/planes" then
          return planes.main( http_request);
        elsif request_uri = "/trains" then
          return trains.main( http_request);
        elsif request_uri = "/vehicles" then
          return vehicles.main( http_request );
        else
          return AWS.Response.Build ("text/html", "<p>Unknown URI");
        
        end if;

   end main_server;

end awsno_cb;
