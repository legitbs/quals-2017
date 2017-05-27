#!/usr/bin/env ruby

require 'fileutils'
require 'digest'

count = (ENV['COUNT'] || 100).to_i

FileUtils.mkdir_p('tmp/alchemy_dist')
FileUtils.mkdir_p('tmp/alchemy_server')

count.times do
  `rm alchemy`
  `ruby tool/gen.rb`
  `crystal build --prelude prelude.cr -s --no-debug --link-flags "--lto -Os" --release -o alchemy src/alchemy.cr src/check.cr`
  `strip alchemy`
  all_out = `./alchemy < tmp/in.txt`
  got = all_out.each_line.to_a.last
  puts got
  want = File.read('tmp/out.txt')
  unless got.include? want
    puts
    puts "FAILED :( :( :("
    exit -1
  end

  d = Digest::SHA256.file('alchemy')
  hd = d.hexdigest
  FileUtils.cp('alchemy', File.join('tmp/alchemy_dist', hd))
  FileUtils.cp('alchemy', File.join('tmp/alchemy_server', hd))
  FileUtils.mv('tmp/in.txt', File.join('tmp/alchemy_server', "#{hd}.in"))
  FileUtils.mv('tmp/out.txt', File.join('tmp/alchemy_server', "#{hd}.out"))
end

Dir.chdir('tmp') do
  `tar jcf alchemy_dist.tar.bz2 alchemy_dist`
  `tar jcf alchemy_server.tar.bz2 alchemy_server`
  FileUtils.mv('alchemy_dist.tar.bz2',
               Digest::SHA256.file('alchemy_dist.tar.bz2').hexdigest + '.tar.bz2')
end
