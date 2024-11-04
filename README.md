# MQ-Match
## Compile
Compiling the code by executing the following command.
    
    make
## Execute
After a successful compilation, the binary file is created under the `build/` directory. Running `match` using the following command.

    ./match -d <data_path> -q <query_path> -u <update_stream_path>
## Format
### date graph
    v <id> <label>
    ...
    e <id1> <id2> <label>
    ...
* Lines starting with "v" represent vertices;
* Lines starting with "e" represent edges.
### stream
    e <id1> <id2> <label>
    ...
### query graph
    c <w1> <w2> <w3>
    t <query_id> <the number of vertices>
    v <id> <label>
    ...
    e <id1> <id2> <label>
    ...
    t <query_id> <the number of vertices>
    v <id> <label>
    ...
    e <id1> <id2> <label>
    ...
* w1 represents the number of common subgraphs;
* w2 represents the number of query graphs;
* w3 represents the number of common edges of all query graphs.
