#!/usr/bin/env ruby

require 'fileutils'
require 'digest'

count = (ENV['COUNT'] || 100).to_i

FileUtils.mkdir_p('tmp/occult_dist')
FileUtils.mkdir_p('tmp/occult_server')

count.times do
  `rm -f occult`
  `ruby tool/gen.rb`
  `csc -o occult -debug-level 0 -no-argc-checks -no-bound-checks -no-trace -fixnum-arithmetic -specialize -unsafe -explicit-use -uses library -no-lambda-info -no-procedure-checks -lfa2 src/main.scm`
  `strip occult`
  all_out = `./occult < tmp/in.txt`
  got = all_out.each_line.to_a.last
  puts got
  want = File.read('tmp/out.txt')
  unless got.include? want
    puts
    puts "FAILED :( :( :("
    exit -1
  end

  d = Digest::SHA256.file('occult')
  hd = d.hexdigest
  FileUtils.cp('occult', File.join('tmp/occult_dist', hd))
  FileUtils.cp('occult', File.join('tmp/occult_server', hd))
  FileUtils.mv('tmp/in.txt', File.join('tmp/occult_server', "#{hd}.in"))
  FileUtils.mv('tmp/out.txt', File.join('tmp/occult_server', "#{hd}.out"))
end

Dir.chdir('tmp') do
  `tar jcf occult_dist.tar.bz2 occult_dist`
  `tar jcf occult_server.tar.bz2 occult_server`
  FileUtils.mv('occult_dist.tar.bz2',
               Digest::SHA256.file('occult_dist.tar.bz2').hexdigest + '.tar.bz2')
end
