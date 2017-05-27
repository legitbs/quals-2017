with AWS.Response;
with AWS.Status;
with Ada.Strings.Unbounded; use Ada.Strings.Unbounded;
with Ada.Containers.Doubly_Linked_Lists;

package trains is
    type train_data is record
        name        : Unbounded_String;
        model       : Unbounded_String;
        t           : unbounded_string;
        speed       : Float;
        passengers  : Integer;
    end record;


   function main (http_request : AWS.Status.Data) return AWS.Response.Data;

end trains;