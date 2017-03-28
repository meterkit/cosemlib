
# Meter system abstraction

The System module abstract all the hardware parts of a meter device and is generic with only pure C.

Implementation differs regarding the build target. Current supported targets are:

  * Linux (pthreads)
  * Windows (pthreads with MinGW)
  * STM32F407 Discovery board

