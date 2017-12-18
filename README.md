# DLMS/Cosem stack

This is an implementation of the DLMS/Cosem protocol in the BSD Open Source and permissive license. This protocol is mainly used in gas/water/electricity meters but is enough generic to target any IoT device.

See the official organization group to learn more: http://www.dlms.com.

This repository provides C code, mainly framing encoding/decoding functions. There is no any integration in a complete stand-alone examples. See other repositories for that purpose.

## Development goals

This Cosem stack has the following goals :

  * Pure portable and stand-alone ANSI C99 code
  * Fully unit tested with pre-defined vectors
  * Client/server implementation, LogicalName referencing, LLS, HLS3, 4 and 5, security policy 1
  * Examples using Cosem over TCP/IP
  * Memory efficient / no dynamic allocation (static, configurable at build-time)
  * Full traces
  * Memory protected against buffer overflow using Array utility

## What is working so far

  * Utilities (buffer utilities)
  * BER coder/decoder
  * Association coders and decoders AARQ/AARE/RLRQ/RLRE (LLS)
  * Secure HLS5 GMAC Authentication
  * Get Request normal/by block
  * Set request normal
  * Action service
  * Exception response in case of problem
  * HDLC framing utility
  * Serial port HAL (Win32/Linux)


# How to view/edit the code

Eclipse CDT project files are available at the root of the repository.

# Manual and integration hints

FIXME: before writing this section, wait for stabilization of the HAL/Cosem API and utilities


# Development schedule

## Version 1.0 TODO

  * LN with ciphering Security Policy 0 (Authenticated & encrypted)
  * HLS 3, 4, 5, 6

## Version X.0

  * Multiple logical devices support
  * ACCESS service
  * GBT service
  * ECDSA
  * ECDSA data transport cyphering
