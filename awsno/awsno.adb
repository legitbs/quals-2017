with Ada.Text_IO;

with AWS.Default;
with AWS.Server;

with awsno_cb;

with Interfaces.C; use Interfaces.C;
with Interfaces.C.Strings;

procedure awsno is

   web_server : AWS.Server.HTTP;

begin
   AWS.Server.Start (web_server, "AwsNo",
                     Max_Connection => 1,
                     Callback       => awsno_cb.main_server'Access,
                     Port           => 9345 );

   delay 30.0;

   AWS.Server.Shutdown (web_server);
end awsno;
