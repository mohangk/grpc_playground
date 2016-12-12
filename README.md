# GRPC is cool 

## What is this ? 

This is a just a directory with some sample code to hack on GRPC code.

## Would this be of any use to me ?

Probably not.

## I don't care, I would like to try to run this code

Ok if you insist. 

### Dependencies are:

  * C++ [protobuf 3.0](https://github.com/google/protobuf/)   
  * C++ [GRPC](https://github.com/grpc/grpc) libraries.
  * C++ [sqlpp11](https://github.com/rbock/sqlpp11) and its [Postgres connector](http://github.com/matthijs/sqlpp11-connector-postgresql)
  * Postgres server & relevant libs, header files


Currently there are  2 projects here that might be of interest to you.

### C++ article_server & article_client

This is a super simple example of creating a GRPC server that can serve data from a database. This is stored within the articleServer directory. 

#### To install & run:

1. cd ./articleStore
2. make
3. ./article_server
4. ./article_client will run the client 

Lots of details have been left out, read the source Luke :)

### Java based GRPC client

Contributed by the awesome [andrisama](https://github.com/andrisama). 

#### To install & run:

Andri will need to complete this readme :)





