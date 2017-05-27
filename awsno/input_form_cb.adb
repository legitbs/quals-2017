with AWS.Parameters;

package body input_form_cb is

   function input_form
     (Request : AWS.Status.Data)
      return AWS.Response.Data
   is
      Text : constant String
        := AWS.Parameters.Get (AWS.Status.Parameters (Request), "text");
   begin
      if Text = "" then
         return AWS.Response.Build
           ("text/html", "<html><body>"
              & "<form>"
              & "<textarea rows=""7"" name=""text"" cols=""48""></textarea>"
              & "<br><input type=""Submit"">"
              & "</form></body></html>");
      end if;

      AWS.Log.Write (Text_Log, Request, Text);

      return AWS.Response.Build
        ("text/html", "<html><body>"
           & "<p>Thanks for you comment <br><pre>"
           & Text & "</pre>"
           & "<form>"
           & "<input type=""Submit"" value=""Back"">"
           & "</form></body></html>");
   end input_form;

end input_form_cb;
