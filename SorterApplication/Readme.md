This problem aims to implement a TCP Server application that sorts a set of integer elements
provided by the Clients. Initially, a Client sends a message containing a total number of
elements, followed by the set of elements (as shown below), to the Server. On receiving the
message, the Server decodes the message and performs sorting (Insertion Sort or any other
kind of sorting) on received elements. Once sorting is completed, it responds to the client,
mentioning the total number of elements followed by the set of sorted elements (as shown).

|----------|---------|---------|-----|---------|
| No of | Elem 1 | Elem 2 | ... | Elem N |
| Elements| | | | |
|----------|---------|---------|-----|---------|
| (2 byteS)|(4 bytes)|(4 bytes)| |(4 bytes)|
|----------|---------|---------|-----|---------|