with AWS.Response;
with AWS.Status;

package awsno_cb is

   function main_server (http_request : AWS.Status.Data) return AWS.Response.Data;

end awsno_cb;
