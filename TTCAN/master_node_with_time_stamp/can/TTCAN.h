#ifndef TTCAN_H
#define TTCAN_H
#ifndef F_CPU
#define F_CPU 16000000 // AVR clock frequency in Hz, used by util/delay.h
#endif
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "MCP2515.h"
#include "TTCAN_Nodes.h"
// Note that the basic protocol configuration happens at the beginning of TTCAN.c
// Internal control signals used in messages from the master node.
#define CYCLE_INFO 1
#define MATRIX 2
void set_spi_in_use(uint8_t u);
uint8_t got_heartbeat();
uint8_t got_message();
// Use the next available arbitration window to send a message with the
// id virtual_ids[i].
void set_arbitration(uint8_t i);
// Some getter&setter functions.
void set_cycle_count(uint8_t i);
void set_slot_count(uint8_t i);
void set_node_id(uint16_t id, uint8_t i);
uint16_t get_node_id(uint8_t i);
uint8_t get_cycle_count();
uint8_t get_slot_count();
uint8_t get_current_basic_cycle();
uint8_t get_current_slot();
// Initialize timer 1. Should trigger ISR after SLOTTIME.
void timer1_init();
// Initialize Timer 2. This timer is used to get better synchronization
// when receiving a heartbeat.
void timer2_init();
// Prepare the Start of Frame Interrupt. Only used by node_init().
void prepare_sof_interrupt();
// Enable RX0BF Interrupt on Pin PJ1 and the RX1BF Interrupt on Pin PK0.
void prepare_rxbf_interrupt();
// Build the node id. Is only used by node_init().
void build_id(uint8_t nodetype, uint8_t number, uint8_t recordme, uint8_t i);
// Send a reference message.
void heartbeat();
// Transmit the system matrix to all slaves.
void transmit_matrix();
// Receive the system matrix.
void receive_matrix();
// Set the acceptance Masks and filters so that heartbeats go to Receive Buffer 0 only.
// All other messages will go to Receive Buffer 1.
void set_masks_and_filters();
// Checks if the ids of this node are in the current schedule. If not,
// they may be sent in an arbitration window.
void check_for_arbitration();
// Initialise the CAN node depending on its task.
uint8_t node_init(uint8_t nodetype[], uint8_t number[], uint8_t recordme[], uint8_t number_of_ids);
// Is transmit buffer buf safe to write to?
uint8_t buffer_ok_to_load(uint8_t buf);
// Safely set the next message to send.
uint8_t ttcan_set_as_next_msg(can_msg msg, uint8_t bufno);
// Read a msg from a buffer and avoid SPI mixups.
// bufno has to be 0 or 1. If bufno is not 0, receive buffer 1
// will be read.
void ttcan_getmsg(can_msg *msg, uint8_t bufno);
/* Look below node_init in TTCAN.c for the following Interrupt service routines:
	Timer 1 interrupt, happens at the beginning of every slot.
	ISR(TIMER1_COMPA_vect)
	
	The interrupt routine for the RX1BF pin PK0, i.e.,
	what happens if you receive a normal message.
	ISR(PCINT2_vect)
	
	The interrupt routine for the RX0BF pin PJ1, i.e.,
	what happens if you receive a heartbeat.
	ISR(PCINT1_vect)
	
	The Start of Frame pin is low.
	ISR(INT5_vect)
*/
#endif /* TTCAN_H */