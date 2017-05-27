with AWS.Response;
with AWS.Status;

package index_write is

   function main (http_request : AWS.Status.Data) return AWS.Response.Data;

end index_write;