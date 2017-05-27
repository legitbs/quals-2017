#!/bin/bash
cp server/liberty-server $1
mkdir $1/kernel
mkdir $1/payloads
mkdir $1/payloads/64
mkdir $1/payloads/64/aes

cp server/payloads/*.bin $1/payloads
cp server/payloads/64/*.bin $1/payloads/64
cp server/payloads/64/aes/*.bin $1/payloads/64/aes

cp server/flag $1
cp server/compress.py $1

