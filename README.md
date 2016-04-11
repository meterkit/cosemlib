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
  * Action service
  * Exception response in case of problem
  * Full extensible/generable database layer to access to Cosem objects


# Examples

The provided example is a server implementation using CosemLib. It includes traditionnal code such as a security library,
a generic date-time code, log book and some registers.

The communication transport is using a TCP/IP layer using a Cosem IP wrapper header. The default port is 4059.
The CosemLib is using one logical device (number 1) and is configured with two associations that have the
following parameters:


|   Name        |     Usage     |  SAP  |  Conformance block
| ------------- |:-------------:|:-----:|:-------------------:
| Public client |  Mandatory    |   16  | Get/Block transfer
| Management    |     R/W       |   1   | Get/Set/Action/Block transfer/Selective Access


# How to build

You will need QtCreator to load the project file. Any version of Qt is good, since we use raw ANSI C and
QtCreator is only here as an nice IDE.

Three targets are available:

  * Debug/Release: traditional ones, the server example is compiled
  * Tests: build and launches stack unit tests

# Basic manual and integration hints

FIXME

# Development schedule

## Version 1.0

  * Provide few complete cosem objects (Association, Clock)
  * HLS GMAC

## Version 1.5

  * Get/Set by block
  * Get/Set with selective access
  * Attributes 0 and 1 managed by the database
  * Security Policy 1 + HAL algorithm layer
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


