#!/bin/sh
# many thanks to @negbie
# https://github.com/google/gopacket/issues/424#issuecomment-369551841
set -ex
apk update
apk add linux-headers musl-dev gcc libpcap-dev ca-certificates git

mkdir -p /go/src/github.com/d3adzo
mkdir -p /mnt/out
cp -a /mnt /go/src/github.com/d3adzo/poetry
cd /go/src/github.com/d3adzo/poetry
go get -v all
go build --ldflags '-linkmode external -extldflags "-static -s -w"' -v ./
cp ./poetry /mnt/out/
