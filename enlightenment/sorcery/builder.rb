require 'fileutils'
require 'digest'

count = (ENV['COUNT'] || 100).to_i

FileUtils.mkdir_p('tmp/sorcery_dist')
FileUtils.mkdir_p('tmp/sorcery_server')

count.times do
  `cargo clean`
  `ruby tool/gen.rb`
  `cargo build --release -j4`
  `strip target/release/sorcery`
  all_out = `./target/release/sorcery < tmp/in.txt`
  got = all_out.each_line.to_a.last
  puts got
  want = File.read('tmp/out.txt')
  unless got.include? want
    puts
    puts "FAILED :( :( :("
    exit -1
  end

  d = Digest::SHA256.file('target/release/sorcery')
  hd = d.hexdigest
  FileUtils.cp('target/release/sorcery', File.join('tmp/sorcery_dist', hd))
  FileUtils.cp('target/release/sorcery', File.join('tmp/sorcery_server', hd))
  FileUtils.mv('tmp/in.txt', File.join('tmp/sorcery_server', "#{hd}.in"))
  FileUtils.mv('tmp/out.txt', File.join('tmp/sorcery_server', "#{hd}.out"))
end

Dir.chdir('tmp') do
  `tar jcf sorcery_dist.tar.bz2 sorcery_dist`
  `tar jcf sorcery_server.tar.bz2 sorcery_server`
  FileUtils.mv('sorcery_dist.tar.bz2',
               Digest::SHA256.file('sorcery_dist.tar.bz2').hexdigest + '.tar.bz2')
end
