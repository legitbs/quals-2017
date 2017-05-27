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

reversed = (1 == rand(2))

File.open(
  "../priv/Cory_Doctorow_-_Down_and_Out_in_the_Magic_Kingdom.txt",
  "r") do |book|

  sample_length = rand(40) + 8

  sample_start = rand(book.size - (2 * sample_length))

  book.seek(sample_start)
  sample = book.read(sample_length).gsub(/\s+/, ' ')
end

File.open("../src/check.cr", "w") do |c|

  check_outs = sample.bytes.each_with_index.map do |b, i|
    c.puts "def check_#{i}(cur : Int64)"
    c.puts "  exit(#{i + 1}) if 0 == cur"
    running = b

    32.times.each do
      operation = [:+ , :-].sample

      if (2 ** 32 < running)
        operation = :%
      end

      operand = rand(2**5) + 3

      result = send operation, running, operand
      c.print "  cur = cur #{operation} #{operand}"
      c.puts " # #{running} #{operation} #{operand} = #{result}"

      running = result
    end

    c.puts "  exit(#{i + 1}) if #{running} != cur"
    c.puts "  return cur"
    c.puts "end"
    c.puts

    running
  end

  c.puts "def check(got : String)"
  if reversed
    c.puts "  bytes = got.reverse.bytes"
  else
    c.puts "  bytes = got.bytes"
  end
  c.puts "  sum = 0"

  sum = 0

  check_outs.each_with_index.each do |v, i|
    c.puts "  cur = bytes[#{i}]"
    c.puts "  sum = sum + check_#{i}(cur.to_i64); # #{sum} + #{v} = #{sum + v}"
    sum += v
    c.puts "  sum = sum >> 3; # #{sum} >> 3 = #{sum >> 3}"
    sum >>= 3
  end

  c.puts "  exit(250) if #{sum} != sum"
  c.puts "  return sum"
  c.puts "end"
end

FileUtils.mkdir_p "../tmp"
File.open("../tmp/in.txt", 'w') do |e|
  if reversed
    e.puts sample.reverse
  else
    e.puts sample
  end
end

File.open("../tmp/out.txt", "w") do |o|
  o.puts sum
end
