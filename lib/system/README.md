
# NOR Flash File System (NFFS)

## Description

This simple file system is to be used with NOR flash usually found in embedded applications.

## Features

NFFS feature list:
  
  * Pure portable and stand-alone ANSI C99 code
  * Fully unit tested
  * RAM physical flash test device (used for tests)
  * Erase/write statistics 
  * POSIX style API (open, close, read, write)
  * Support flat and FIFO access
  * Power failure safe
  * User-defined per-file signature
  * RAM cache
  
Limitations:

  * 1MB per file

# Manual

## General design

This file system provides very low level API and file abstraction so that you can fine tune the usage to your needs. The API functions are thread-safe 
so you can use it with any RTOS available on the market (wrappers are used).

The FS support two kinds of files:
  
  * Flat structure, typically configuration files. The FS uses two (or more!) areas and always loads the newest clean data (usage of a counter). It is called 
    an 'S' file type in this documentation
  * FIFO, for your data logger, it is called an 'F' file type in this documentation

There is no wear levelling, instead user must carefully configure the files to not go out of the device specification (erase limitation). Simulation can 
be performed thanks to the RAM test device.

## Block definition

A block is the minimal area that can be erased in the device (== all bytes set to 0xFF). This block size is configurable. Try to not use a too small 
value (ie, 256 bytes), even if your memory device support it because the FS is storing a header for each block stored. Typical value is 4KB but it can 
be adjusted to your needs.

## Data cache

A block is always read fully even if only one byte is asked to be read by the application. This allow faster data read access (physical access to memory 
is a bottle-neck).


## Device mapping

A configuration file must be written (or generated!) to indicate where the files are physically allocated. To ease the mapping, we do not talk in bytes
but in block number, starting at zero.

Let's take a simple example with two files declarated, a Structure and a FIFO. 

                                        block          Info byte
    -------------------------          
    |          S(1)          |           0               1
    ------------------------- 
    |          S(2)          |           1               0
    -------------------------
    |          S(3)          |           2               0
    -------------------------
    |          F             |           3
    -------------------------
    |          F             |           4
    -------------------------
    |          F             |           5
    -------------------------
    |          F             |           6
    -------------------------

## Block header

The header of each block consist of the following elements

    typedef struct
    {
    uint8_t signature[20];  //< Optional signature of the data, written when the block is full FIXME: make the size configurable
    uint32_t counter;       //< Counter written when the block is full
    uint16_t name;          //< File name (for consistency checks)
    uint16_t size;          //< Physical data size used for signature
    uint8_t next;           //< Next logical block number (offset from the first absolute block number)
    uint8_t info;           //< Block information
    } fs_header;

## Case of small data size

A small data size is a structure that can fit in one block. Otherwise, the structure is using more than one block.

The number of blocks used is determined by the following formula:

    nb_blocks =  div_up(sizeof(struct)/block_payload_size)

With the following parameters:

  * block_payload_size = block_size - header_size
  * struct = application data structure
  * div_up = division function rounded up

## Information byte

Here is the bit string meaning of the 'info'  


     bit       7    6    5    4   3   2   1   0
     value     1    1    1    1   1   1   1   FB

FB: First Block, set to 0 when the block is the first block of the data structure, otherwise 1 (structure continuation) 


## Dynamic design execution

This file system has no any thread protection because it is designed to be used by one context only. Therefore, you should use a dedicated 
thread to call any of the API function.



