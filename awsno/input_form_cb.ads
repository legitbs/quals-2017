with AWS.Log;
with AWS.Response;
with AWS.Status;

package input_form_cb is

   Text_Log : AWS.Log.Object;

   function input_form (Request : AWS.Status.Data) return AWS.Response.Data;

end input_form_cb;