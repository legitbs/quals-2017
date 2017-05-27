require 'fileutils'

Dir.chdir(File.dirname(__FILE__))

sample = nil
sum = nil

def +(a, b)
  a + b
end
def -(a, b)
  a - b
end
def *(a, b)
  a * b
end
def /(a, b)
  r = a / b
  return r if a >= 0

  (a.to_f / b).ceil
end
def %(a, b)
  return a.remainder(b)
end

File.open(
  "../priv/Cory_Doctorow_-_Down_and_Out_in_the_Magic_Kingdom.txt",
  "r") do |book|

  sample_length = rand(40) + 8

  sample_start = rand(book.size - (2 * sample_length))

  book.seek(sample_start)
  sample = book.read(sample_length).gsub(/\s+/, ' ')
end

File.open("../Sources/check.swift", "w") do |c|
  c.puts "import Glibc"
  c.puts

  check_outs = sample.bytes.each_with_index.map do |b, i|
    c.puts "func check_#{i}(got: Int64) -> Int64 {"
    c.puts "  var cur = got"
    c.puts "  if (0 == cur) { exit(#{i + 1}) }"
    running = b

    32.times.each do
      operation = %i{+ -}.sample

      if (2 ** 32 < running)
        operation = :%
      end

      operand = rand(2**5) + 3

      result = send operation, running, operand
      c.print "  cur = cur #{operation} #{operand}"
      c.puts " // #{running} #{operation} #{operand} = #{result}"

      running = result
    end

    c.puts "  if (#{running} != cur) { exit(#{i + 1}) }"
    c.puts "  return cur"
    c.puts "}"
    c.puts

    running
  end

  c.puts "public func check(got: String) -> Int64 {"
  c.puts "  let buf = [UInt8](got.utf8)"
  c.puts "  var sum: Int64 = 0;"

  sum = 0

  check_outs.each_with_index.each do |v, i|
    c.print "  sum = sum + check_#{i}(got: Int64(buf[#{i}]))"
    c.puts  "  // #{sum} + #{v} = #{sum + v}"
    sum += v
    c.puts "  sum = sum >> 3; // #{sum} >> 3 = #{sum >> 3}"
    sum >>= 3
  end

  c.puts "  if (#{sum} != sum) { exit(250) }"
  c.puts "  return sum"
  c.puts "}"
end

FileUtils.mkdir_p "../tmp"
File.open("../tmp/in.txt", 'w') do |e|
  e.puts sample
end

File.open("../tmp/out.txt", "w") do |o|
  o.puts sum
end
