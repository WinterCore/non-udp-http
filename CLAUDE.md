# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a learning project to build an HTTP/1.1 server from scratch in C. The implementation follows RFC 7230-7235 specifications.

**Important**: The user is building this server to learn. When helping:
- Explain concepts in detail with examples
- Do NOT provide direct code solutions
- Guide toward understanding rather than implementing
- Answer questions about HTTP protocol details, RFC specifications, and debugging approaches

## Build System

The project uses a Makefile with the following targets:

```bash
# Build debug version with symbols and no optimization
make debug

# Build release version with optimizations
make release

# Build default (Server target)
make

# Clean build artifacts
make clean
```

The build system automatically finds all `.c` files in the project and compiles them into a single `Server` executable.

Build flags:
- Debug builds: `-ggdb -O0 -DDEBUG`
- Release builds: `-DNDEBUG`
- All builds: `-std=c11 -Wall -Wextra`

## Project Structure

This is an early-stage project. Currently contains:
- `src/main.c` - Entry point (currently minimal)

The architecture is being actively developed from the ground up.

## HTTP/1.1 Specifications

The server targets HTTP/1.1 only, following these RFCs:
- RFC 7230: Message Syntax and Routing
- RFC 7231: Semantics and Content
- RFC 7232: Conditional Requests
- RFC 7233: Range Requests
- RFC 7234: Caching
- RFC 7235: Authentication

When discussing HTTP protocol details, refer to these specifications and explain the reasoning behind design decisions.
