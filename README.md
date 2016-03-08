# DLMS/Cosem stack

This is an implementation of the DLMS/Cosem protocol in the BSD Open Source and permissive licence.

## Developement goals

This Cosem stack has the following goals :

  * Pure portable and standalone ANSI C99 code
  * Fully unit tested with pre-defined vectors
  * Client/server implementation, LogicalName referencing, HLS/security policy 1
  * Examples using Cosem over TCP/IP
  * Memory efficient / no dynamic allocation (static, configurable at build-time)
  * Full traces and memory protected against buffer overflow

## What is working

  * Utilities (buffer utilities)
  * BER coder/decoder
  * Basic association (LLS)

## TODO / Development schedule

  * Database access layer
  * Provide few cosem objects (Association, Security Setup, Clock, Profile)
  * Get/Set request
  * Get/Set by block
  * Object List
  * HLS/ Security Policy 1 + HAL algorithm layer
