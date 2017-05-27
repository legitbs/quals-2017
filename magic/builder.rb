#!/usr/bin/env ruby

require 'fileutils'
require 'digest'

count = (ENV['COUNT'] || 100).to_i

FileUtils.mkdir_p('tmp/magic_dist')
FileUtils.mkdir_p('tmp/magic_server')

count.times do
  `rake clean`
  `rake`
  `strip magic`
  all_out = `./magic < tmp/in.txt`
  got = all_out.each_line.to_a.last
  puts got
  want = File.read('tmp/out.txt')
  unless got.include? want
    puts
    puts "FAILED :( :( :("
    exit -1
  end

  d = Digest::SHA256.file('magic')
  hd = d.hexdigest
  FileUtils.cp('magic', File.join('tmp/magic_dist', hd))
  FileUtils.cp('magic', File.join('tmp/magic_server', hd))
  FileUtils.mv('tmp/in.txt', File.join('tmp/magic_server', "#{hd}.in"))
  FileUtils.mv('tmp/out.txt', File.join('tmp/magic_server', "#{hd}.out"))
end

Dir.chdir('tmp') do
  `tar jcf magic_dist.tar.bz2 magic_dist`
  `tar jcf magic_server.tar.bz2 magic_server`
  FileUtils.mv('magic_dist.tar.bz2',
               Digest::SHA256.file('magic_dist.tar.bz2').hexdigest + '.tar.bz2')
end
