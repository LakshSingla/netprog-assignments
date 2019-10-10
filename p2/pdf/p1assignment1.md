# Shell

Submitted to: Dr. Hari Babu\
Course: Network Programming IS F462\
Assignment 1

## Design
This exercise create a bash like shell primitive support for chaining processes via pipes, shared memory and message queues. Functionality for daemonizing a process, redirection and limited job control is also provided.

The following figure gives an illustration of the design we incorporated.

<p align = "center">
<img src = "./cs1.png" alt = "client-server" width = "700" />
</p>

### Shell Functionality

The main shell process is a prompting process, which shows the prompt and waits for user to enter the command.

On successfully recieving the command, it passes the command to a newly spawned the child process in a seperate process group. This child process is the leader of the process group, and all of the subprocesses associated with the command will be present in this group for job control. This process acts as the leader for the subprocesses. The prompting parent process checks if & is present at the end or not, and if not, gives the terminal control to this group (to run in the fg). Parent then waits for the leader to finish, or stop executing.

The leader now parses the command and sees the relevant message passing techniques used between different subprocesses. It also sees the redirection operators. For each of the subprocess, it maintains a relevant structure denoting its input fd, and ouput fd.

This loader now starts the sequential execution of the processes by exec() and fork() and waits for each of them to finish. It sets up appropriate fds before executing.

### Daemonize Functionality

The daemonize <process name> command helps in daemonizing the process by making sure that the child process which will be responsible for exec of the <process name> will be immune to signals such as SIGHUP, be devoid of any file descriptors, have its STDIN, STDOUT, STDERR set to /dev/null, and umask set to 0. Also, it will be in its own private session, and won't be a session leader, therefore won't be able to reclaim any terminal device.


### Job control Functionality

Each job's leader is the process group leader as well. The shell maintains a list of jobs/commands, and responds to the change of state of each of the child. Due to this, it keeps the track of current foreground controlling group, and background groups. Also it maintains which all jobs are in running state and stopped state.
`jobs` is used to display the current list of jobs
`bg` is used to run a background job in background (without giving it access to the controlling terminal)
`fg` is used to run a background job in foreground (by setting it as the terminal controlling process)

As soon as a node (client) starts the shell on their machine, a TCP connection is requested and established with the server (which creates a new child process to handle the connection), and the shell is then ready to accept commands from the user. There is also a child process created on the client, which waits for an other connection to be established with the server. This child process is responsible for executing the commands that are received from the server and are required to be executed on the given machine.\
When the user enters the command on the shell, it is sent to the server. On the server, the command is first parsed to break it into sequence of (command, node number) pairs, so that the server knows which command is to be executed on which node. The output from all the nodes is collected by the server and the result is sent to the command executing node.

The following figure shows how an example command would run on this architecture.

<p align = "center">
<img src = "./cs2.png" alt = "client-server-example" width = "900" />
</p>

### Lifecycle of a command execution

- **When server starts** -
It creates a socket and starts listening for connections from clients. It works like a concurrent server, where a new process is spawned to handle each new connection. When a connection is established with a client, server waits to read a message from the client.

- **When client starts** -
It tries to establish connection with the server (i.e. server should be running before trying to run the client). After the connection is established, it starts running a shell to accept commands from the user. The client also spawns a child process in the beginning, which creates a socket and starts listening for connection from the server. This process would be responsible for executing commands whose output is required by other nodes (clients).

- **When user enters a command on the client shell** -
The command is sent to the server via the already established connection. The shell then waits to read the output from server on this connection.

- **When server receives command from the client** -
First it parses the command to break it at pipes (`|`) creating smaller sub-commands and to determine which sub-command is to be sent to which client for execution (via name-IP pairs specified in CONFIG file). After parsing, the sub-commands are sent to the respective IP addresses, via a connection where the client acts as the passive end.

- **When client receives command from the server** -
Client runs the received sub-command using `popen()`. The output is first written into a pipe, which is then read and sent back to the server.

- **After server receives outputs from all the clients** -
It combines them as required and sends them to the original client that ran the main command.

- **After client receives output from the server** -
The output is displayed for the user to see and the shell is ready to accept another command.



## Usage


## Screenshots
