# Assignment 3 - P1

Submitted to: Dr. Hari Babu\
Course: Network Programming IS F462\
Submitted by: **Kunal Mohta (2017A7PS0148P)** & **Laksh Singla (2017A7PS0082P)**

## Design

- User is given the facility to join and leave a group.
- User can send notification to a joined group only.
- I/O Multiplexing using the *select()* call is used to handle the availability of data from *standard input* and from all the joined group file descriptors.
- For sending notification, blocking call for *sendto()* is required, which is done by creating a child process using *fork()*.
- For the join and leave group operations, no blocking call is required. Hence, they are carried out as is.

## Usage
Compiling and running -
```
make
./a.out
```

Supported commands -
```
// join a group
join <group-name>

// send notification to a group
notify <group-name>

// leave a group
leave <group-name>

// get list of joined groups
joined-groups
```

## Assumptions
- List of courses is already known and each group has already been assigned a multicast group ip.
- There can be no additions/deletions to the list of courses while the code is running.
- Change `COURSE_COUNT` and `course_list` when adding/deleting courses.
- The program is supposed to be run on different machines. If it is run in 2 different terminals on the same machine, then joining a group in 1 terminal would result in it being joined in the other terminal as well.