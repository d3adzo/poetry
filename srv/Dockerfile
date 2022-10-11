FROM golang:alpine

WORKDIR /poetry

COPY . .

RUN go get -v all

RUN go build

ENV IFACE=eth0

ENTRYPOINT ["./poetry"]
