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
  * Basic association (LLS)
  * Get Request of the Clock object

## Version 1.0 development schedule

  * Database access layer
  * Provide few cosem objects (Association, Security Setup, Clock, Profile)
  * Get/Set request normal
  * Get/Set by block
  * Get/Set with selective access
  * Object List
  * HLS GMAC / Security Policy 1 + HAL algorithm layer

## Version 2.0

  * ACCESS service
  * GBT service
  * ECDSA + SHA-256 HLS
  * ECDSA data transport cyphering


