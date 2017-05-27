require 'fileutils'
require 'digest'

count = (ENV['COUNT'] || 100).to_i

FileUtils.mkdir_p('tmp/witchcraft_dist')
FileUtils.mkdir_p('tmp/witchcraft_server')

count.times do
  `swift build --clean`
  `ruby tool/gen.rb`
  `swift build -c release`
  `strip .build/release/witchcraft`
  all_out = `./.build/release/witchcraft < tmp/in.txt`
  got = all_out.each_line.to_a.last
  puts got
  want = File.read('tmp/out.txt')
  unless got.include? want
    puts
    puts "FAILED :( :( :("
    exit -1
  end

  d = Digest::SHA256.file('./.build/release/witchcraft')
  hd = d.hexdigest
  FileUtils.cp('.build/release/witchcraft',
               File.join('tmp/witchcraft_dist', hd))
  FileUtils.cp('.build/release/witchcraft',
               File.join('tmp/witchcraft_server', hd))
  FileUtils.mv('tmp/in.txt',
               File.join('tmp/witchcraft_server', "#{hd}.in"))
  FileUtils.mv('tmp/out.txt',
               File.join('tmp/witchcraft_server', "#{hd}.out"))
end

Dir.chdir('tmp') do
  `tar jcf witchcraft_dist.tar.bz2 witchcraft_dist`
  `tar jcf witchcraft_server.tar.bz2 witchcraft_server`
  FileUtils.mv('witchcraft_dist.tar.bz2',
               Digest::SHA256.file('witchcraft_dist.tar.bz2').hexdigest + '.tar.bz2')
end
