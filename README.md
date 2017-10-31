# DLMS/Cosem stack

This is an implementation of the DLMS/Cosem protocol in the BSD Open Source and permissive license. This protocol is mainly used in gas/water/electricity 
meters but is enough generic to target any IoT device.

See the official organization group to learn more: http://www.dlms.com.

## Development goals

This Cosem stack has the following goals :

  * Pure portable and stand-alone ANSI C99 code
  * Fully unit tested with pre-defined vectors
  * Client/server implementation, LogicalName referencing, HLS/security policy 1
  * Examples using Cosem over TCP/IP
  * Memory efficient / no dynamic allocation (static, configurable at build-time)
  * Full traces and memory protected against buffer overflow

## What is working so far

  * Utilities (buffer utilities)
  * BER coder/decoder
  * Basic association AARQ/AARE/RLRQ/RLRE (LLS)
  * Secure HLS5 GMAC Authentication
  * Get Request normal of the Clock object
  * Set request normal
  * Action service
  * Exception response in case of problem
  * Full extensible database layer to access to Cosem objects (can be generated)
  * Basic HDLC layer
  * Basic Cosem client example with modem support and reading configuration parameters
  * Serial port HAL (Win32/Linux)

# Examples

The provided example is a server implementation using CosemLib. It includes traditional code such as a security library,
a generic date-time code, log book and some registers.

The communication transport is using a TCP/IP layer using a Cosem IP wrapper header. The default port is 4059.
The CosemLib is using one logical device (number 1) and is configured with two associations that have the
following parameters:


|   Name        |     Usage     |  SAP  |  Conformance block
| ------------- |:-------------:|:-----:|:-------------------:
| Public client |  Mandatory    |   16  | Get/Block transfer
| Management    |     R/W       |   1   | Get/Set/Action/Block transfer/Selective Access


# How to build

The following make targets are available:

  * make client
  * make server 

# How to view/edit the code

Eclipse CDT project files are available at the root of the repository.

# Manual and integration hints

FIXME: before writing this section, wait for stabilization of the HAL/Cosem API and utilities

## Component organisation

FIXME insert a diagram

## Hardware abstraction layers

### OS abstraction

### File system abstraction

# Development schedule

## Version 1.0 TODO

  * Attributes 0 and 1 managed by the database
  * LN with ciphering Security Policy 0 (Authenticated & encrypted)
  * Get by block (Object List reading)
  * Client implementation (+ TCP and HDLC transport layers)
  * HLS 3 and 4
  
## Version 2.0

  * Cosem studio: client integration and Lua scripting  
  * Cosem studio: database generator
  * Cosem studio: simple scripting client
  * Cosem studio: packet analysis GUI for HDLC and Cosem
  
## Version 3.0

  * File system HAL (load/save)
  * System clock management with windows/linux layer
  * Component system integration (start, stop, suspend, resume)
  * DateTime Next occurrence algorithm
  * Database generation from a JSON Companion Standard (Cosem objects model)
  * Generic profile implementation
  * Generic DateTime implementation
  * Get with selective access
  * Set example
  * Test vectors using BlueBook examples and CTT packets

## Version 4.0

  * Multiple logical devices support
  * ACCESS service
  * GBT service
  * ECDSA + SHA-256 HLS
  * ECDSA data transport cyphering
