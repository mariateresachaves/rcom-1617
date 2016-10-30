/*! \file */
#ifndef DATALINK_H
#define DATALINK_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "utilities.h"

/**
 * @brief Function to print flag content.
 * @param flags - flag content.
 * @param type - flag name.
 */
void printFlags(char * flags, char * type);

/**
 * @brief Function to define newtio.c_cflag depending on the baudrate.
 */
void def_c_cflag();

/**
 * @brief Function to open the port and to establish the connection between transmitter and receiver.
 */
void llopen();

/**
 * @brief Function to disconnect transmitter and receiver connection and to close the port.
 */
void llclose();

/**
 * @brief Function to generate the bcc value for a given data.
 * @param buf - data.
 * @param size - data size.
 */
void bcc_generator(char * buf, int size);

/**
 * @brief Function that guarantee the transparency. Byte stuffing technique applied to a given data.
 * @param buf - data.
 * @param buf_size - data size.
 */
void stuffing(char * buf, int * buf_size);

/**
 * @brief Function to write a packet from transmitter or from receiver.
 * @param side - tells which side is (transmitter or receiver).
 * @param type - type of the packet to write.
 * @param data - data to send with the packet.
 * @param size - data size.
 * @return
 */
int write_packet(char side, char * type, char * data, int size);

/**
 * @brief State machine to write a command.
 * @param side - tells which side is (transmitter or receiver).
 * @param type - type of the command to write.
 * @param data - data to send with the command.
 * @param size - data size.
 * @return
 */
int sm_command(char side, char * type, char * data, char size);

/**
 * @brief State machine to write a message.
 * @param side - tells which side is (transmitter or receiver).
 * @param type - type of the command to write.
 * @param data - data to send with the command.
 * @param size - data size.
 * @return
 */
int sm_write(char side, char * type, char * data, char size);

/**
 * @brief Function to send a information packet with the state machine that write's a message.
 * @param packet - packet to send.
 * @param packet_size - packet size.
 */
void llwrite(char * packet, int packet_size);

#endif
