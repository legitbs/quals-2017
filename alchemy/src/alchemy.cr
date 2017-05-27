require "./check"

puts "enter code:"
STDOUT.flush

code = gets

exit(250) if code.nil?

sum = check(code)

puts "sum is #{sum}"
