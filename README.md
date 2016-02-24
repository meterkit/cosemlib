# DLMS/Cosem stack

This is an implementation of the DLMS/Cosem protocol in Open Source and permissive licence.

## Developement goals

This Cosem stack has the following goals :

  * Pure portable and standalone ANSI C99 code
  * Fully unit tested with pre-defined vectors
  * Client/server implementation
  * Examples using Cosem over TCP/IP
  * Memory efficient / no dynamic allocation (static, configurable at build-time)
  * Full traces and memory protected against buffer overflow


## Development schedule

  * Utilities (buffer utilities)
  * BER coder/decoder
  * Association (LLS)
  * Database access layer
  * Get/Set request
  * HLS/ Security Policy 1
