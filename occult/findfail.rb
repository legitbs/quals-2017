n = 0
loop do
  `rake clean`
  `rake`
  all_out = `./magic < tmp/in.txt`
  got = all_out.each_line.to_a.last
  puts got
  want = File.read('tmp/out.txt')
  break unless got.include? want
  puts n if 0 == (n % 100)
  n += 1
end
