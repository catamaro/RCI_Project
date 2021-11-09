# Distributed Key Table
 
A distributed key table is a simplified version of a distributed hash table. A hash table is a data structure consisting of pairs (key, value), where the key is an integer and the value is a reference to an object associated with that key. Each value is searched for its key. For example, keys can be citizen cards numbers and the values can be records with the name, date of birth, gender and address of the card holders.

Generally, the key space is too large for efficient searching. One then resorts to a hash function that maps arbitrary keys into other keys belonging to a small set of integers.

In a distributed hash table, the pairs (key, value) are distributed among several servers so that the search of a key is efficient and the entry and exit of a server does not change the pairs (key, value) associated with most servers. 

This project creates such structure in a ring formation where the servers communicate through their successor's.

## User Interface

The user interface enables the following commands:

• new i - Creation of a new ring composed exclusively by the i-server.

• entry i boot boot.IP boot.TCP - Server i enters the ring to which the _boot_ server belongs with address boot.IP and port boot.TCP

• sentry i succi succi.IP succi.TCP - Entry of server i into the ring in which server succi with IP address succi.IP and port succi.TCP will be the successor of server i. This command assumes that it is known that the key i is not used by a server belonging to the ring and that succi will be the successor of server i. It is used to build a ring, a server at a time, without the key search.

• leave - Server leaves the ring.

• show - Shows the server status, including its key, IP address and port, as well as the corresponding values of its successor and second successor.

• find k - Searchs the server storing the key k, with the presentation of its key, IP address and port.

• exit - Application closes.
