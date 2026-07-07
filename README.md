# EmberDB-tcp

**EmberDB-tcp** is a Redis-inspired TCP key-value store written in C. It uses POSIX sockets, a custom chained hashmap, a command parser, disk-backed persistence, and `poll()`-based multi-client support.

The goal of this project is to build a small database server from the ground up while learning low-level networking, memory management, file persistence, and systems programming in C.

---

## Features

* TCP server written in C
* Supports multiple clients using `poll()`
* Custom hashmap implementation with separate chaining
* Basic key-value storage commands
* Disk-backed persistence using a backup file
* Command parser for client input
* Client help menu loaded from a text resource file
* Manual memory management
* Modular project structure with headers and source files
* Makefile-based build system

---

## Project Structure

```text
EmberDB-tcp/
├── Makefile
├── README.md
├── backup/
│   └── Backup.txt
├── build/
│   └── EmberDB-tcp
|   | ... 
├── include/
│   ├── commands.h
│   ├── general.h
│   ├── hashmap.h
│   ├── persistance.h
│   └── server.h
├── resources/
│   └── Commands.txt
└── src/
    ├── commands.c
    ├── general.c
    ├── hashmap.c
    ├── main.c
    ├── persistance.c
    └── server.c
```

---

## Build

From the project root, run:

```bash
make
```

This compiles the project and places the executable inside the `build/` directory.

To clean the build files:

```bash
make clean
```

---

## Run

Start the server with:

```bash
make run
```

By default, the server listens on port:

```text
3200
```

---

## Connect to the Server

You can connect using `nc`:

```bash
nc localhost 3200
```

Once connected, you can enter commands directly into the terminal.

---

## Commands

### `PING`

Checks if the server is running.

```text
PING
```

Response:

```text
PONG
```

---

### `SET <key> <value>`

Stores a key-value pair.

```text
SET name Jayce
```

Response:

```text
OK
```

---

### `GET <key>`

Retrieves the value stored at a key.

```text
GET name
```

Example response:

```text
Jayce
```

If the key does not exist:

```text
(nil)
```

---

### `EXISTS <key>`

Checks whether a key exists.

```text
EXISTS name
```

Example response:

```text
Exists
```

or:

```text
Does not Exist
```

---

### `DEL <key>` / `DELETE <key>`

Deletes a key-value pair.

```text
DEL name
```

Example response:

```text
Key Deleted
```

or:

```text
Key Not Deleted
```

---

### `SAVE`

Writes the current in-memory database to the backup file.

```text
SAVE
```

Response:

```text
BACKUP CREATED
```

The backup is saved to:

```text
backup/Backup.txt
```

---

### `BACKUPWIPE`

Deletes the saved backup data.

```text
BACKUPWIPE
```

Response:

```text
BACKUP DATA DELETED
```

---

### `MAXWIPE`

Clears the current in-memory database.

```text
MAXWIPE
```

Response:

```text
DATA DELETED
```

This clears the active database in memory. It does not necessarily remove saved backup data unless the backup is also wiped.

---

### `HELP`

Displays the command list from the resource file.

```text
HELP
```

The command menu is loaded from:

```text
resources/Commands.txt
```

---

### `QUIT`

Disconnects the current client from the server.

```text
QUIT
```

Response:

```text
BYE
```

---

### `END`

Shuts down the server.

```text
END
```

Response:

```text
SERVER CLOSING GOODBYE
```

Unsaved in-memory data will be lost unless `SAVE` was used before shutting down.

---

## Persistence

EmberDB-tcp stores data in memory while the server is running.

The `SAVE` command writes the current hashmap contents to:

```text
backup/Backup.txt
```

When the server starts, it attempts to load existing backup data from this file.

The backup format stores each key-value pair as a line in the file:

```text
key~value
```

When loading, EmberDB-tcp rebuilds the hashmap from the saved key-value pairs.

---

## Multi-Client Support

EmberDB-tcp supports multiple clients using the `poll()` system call.

The listening server socket and active client sockets are stored in a `pollfd` array. The server waits for activity on any socket and responds when a client connects, sends data, disconnects, or shuts down the server.

Conceptually:

```text
Client 1 ─┐
Client 2 ─┼── TCP Server + poll() ── Command Parser ── Hashmap
Client 3 ─┘                                      │
                                                 └── Backup.txt
```

This allows multiple clients to interact with the same in-memory database without creating a new process or thread for each connection.

---

## Technical Overview

EmberDB-tcp is built around several core modules:

### Server

Handles socket creation, binding, listening, accepting clients, and managing multiple connections with `poll()`.

### Commands

Parses user input from clients and routes commands such as `SET`, `GET`, `DEL`, `SAVE`, and `QUIT`.

### Hashmap

Stores key-value pairs in memory using a custom chained hashmap.

### Persistence

Handles saving database contents to disk and loading saved data when the server starts.

### General Utilities

Contains helper functions such as printing text files to the terminal or sending command instructions to clients.

---

## Example Session

```text
$ nc localhost 3200

PING
PONG

SET language C
OK

GET language
C

EXISTS language
Exists

DEL language
Key Deleted

GET language
(nil)

QUIT
BYE
```

---

## Future Improvements

Possible future additions include:

* Support for values containing spaces
* Manual `LOAD` command
* `KEYS` command to list stored keys
* `COUNT` command to count stored keys
* More detailed error messages
* Configuration file for port and max clients
* Improved persistence format
* Safer handling of very large inputs
* Optional logging system

---

## Purpose

This project was built as a systems programming exercise to better understand how database-like servers work internally.

It focuses on:

* C programming
* TCP sockets
* Event-driven server design
* File descriptors
* `poll()`
* Custom data structures
* Manual memory management
* Persistence
* Modular project organization

---

## License

This project is open for educational and personal use.
