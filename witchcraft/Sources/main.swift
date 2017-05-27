import Glibc

print("enter code:")
fflush(stdout)

let code = readLine(strippingNewline: true)!

let sum: Int64 = check(got: code)

print("sum is \(sum)")
