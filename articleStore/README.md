# Testing GRPC vs JSON

## DB schema

DDL as articles.sql. Load into PostgreSQL and load in test data. 

## C++ db access layer

Uses the sqlpp11 lib. Code stored in db/. db/articles.h is generated based on sqlpp11 ddl2cpp script as follows.

```
 ~/path/to/ddl2cpp ./articles.sql ./db/articles DB
```

## Python JSON implementation

### Start python server

## Size of payload

Comparing gzipped JSON vs gzipped protobuf

## Random notes
1. Enabling gzip compression on curl
`curl -H 'Accept-Encoding: gzip' localhost:5000`
1. Enabling debug mode
`-g -O0`

## TODO

1. Find the memory leak in article_server
  1. Best ways to trace memory leaks
1. Try 2 way streaming
1. C, C++ when returning, pass by reference or pass by value?
1. Figure out how the threading model works with the server, how can i log every time a client connects and trace
1. Connection pooling for postgres --> shared between threads ?
1. Find out more about GRPC server threadpool configuration
  1. Find out how many threads a process is running
1. Try timestamp.proto

## GRPC notes

1. Information on GRPC threading pool https://groups.google.com/forum/?utm_medium=email&utm_source=footer#!msg/grpc-io/Cul6fd7cOB0/bMP5f9mdBAAJ
1. Location transparency is not something GRPC aspires to have
1. Discoverability - zookeeper
1. Retries - backoff model
1. Client side loadbalancing
1. Error handling - https://github.com/google/googleapis/blob/master/google/rpc/status.proto
1. Compression - http://stackoverflow.com/questions/36853261/how-to-do-payload-compression-in-grpc/36877341#36877341
