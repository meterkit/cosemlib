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

## What is working so far

  * Utilities (buffer utilities)
  * BER coder/decoder
  * Basic association AARQ/AARE/RLRQ/RLRE (LLS)
  * Get Request normal of the Clock object
  * Set request normal
  * Exception response in case of problem
  * Full extensible/generable database layer to access to Cosem objects

# Development schedule

## Version 1.0

  * Action service
  * Provide few complete cosem objects (Association, Security Setup, Clock)
  * HLS GMAC / Security Policy 1 + HAL algorithm layer

## Version 1.5

  * Get/Set by block
  * Get/Set with selective access
  * Attributes 0 and 1 managed by the database
  * More Cosem objects (Profile, ObjectList of association)

## Version 2.0

  * Test vectors using BlueBook examples and CTT packets
  * Cosem studio (database generator)
  * Cosem studio (simple scripting client)

## Version 3.0

  * Multiple logical devices support
  * ACCESS service
  * GBT service
  * ECDSA + SHA-256 HLS
  * ECDSA data transport cyphering


