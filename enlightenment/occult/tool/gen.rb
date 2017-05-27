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

File.open("../src/check.scm", "w") do |c|
  c.puts "(require-extension srfi-13)"

  check_outs = sample.bytes.each_with_index.map do |b, i|
    c.puts "(define check_#{i} (lambda (cur) (begin"
    c.puts "  (if (= 0 cur) (exit #{i + 1}))"
    running = b

    32.times.each do
      operation = %i{+ -}.sample

      if (2 ** 32 < running)
        operation = :%
      end

      operand = rand(2**5) + 3

      result = send operation, running, operand
      c.print "  (set! cur (#{operation} cur #{operand}))"
      c.puts " ; #{running} #{operation} #{operand} = #{result}"

      running = result
    end

    c.puts "  (if (not (= #{running} cur)) (exit #{i + 1}))"
    c.puts "  cur"
    c.puts ")))"
    c.puts

    running
  end

  c.puts "(define check (lambda (got) (begin"
  c.puts "  (set! sum 0)"
  if reversed
    c.puts "  (set! bytes (map char->integer (string->list (string-reverse got))))"
  else
    c.puts "  (set! bytes (map char->integer (string->list got)))"
  end
  sum = 0

  check_outs.each_with_index.each do |v, i|
    c.puts "  (set! cur (list-ref bytes #{i}))"
    c.puts "  (set! sum (+ sum (check_#{i} cur))) ; #{sum} + #{v} = #{sum + v}"
    sum += v
    c.puts "  (set! sum (arithmetic-shift sum -3)) ; #{sum} >> 3 = #{sum >> 3}"
    sum >>= 3
  end

  c.puts "  (if (not (= #{sum} sum)) (exit 250))"
  c.puts "  sum"
  c.puts ")))"
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
