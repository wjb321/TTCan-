#include "TTCAN.h"
#define ISMASTER 0
// The length of a time slot in µs.
#define SLOTTIME 170
// Number of slots and basic cycles in the system matrix.
// Is used by the master node to send to every other node.
// Number of Basic Cycles should be 2^n according to the International Standard ISO 11898-4
#define BASIC_CYCLES_HARD 2
#define SLOTS_HARD 5
#if ISMASTER
// This is the system matrix. It has to be constructed manually right now. A Matrix could also be read
// from a file, since the slaves wait for the master during initialization anyway.
uint16_t msg_id[SLOTS_HARD * BASIC_CYCLES_HARD] = {0, 1858, 1602, 0,0,0,1859,1601, 0, 0};
uint8_t is_exclusive_window[SLOTS_HARD * BASIC_CYCLES_HARD] = {1,1,1,0,0,1,1,1,0,0};
#else
uint16_t msg_id[128];
uint8_t is_exclusive_window[128];
#endif
// The ids of this node.
uint16_t virtual_ids[3];
// The message buffer.
can_msg messages_to_send[3];
// Flags. Does message i wait to be loaded into the transmit buffer?
volatile uint8_t load_buffer_flag[3] = {0,0,0};
// Flags. arbitration[i] is 1 if id virtual_ids[i] is allowed to be transmitted in an arbitration window.
uint8_t arbitration_allowed[3] = {1, 1, 1};
// Flags. use_next_arbitration_window[i] is 1 if the next arbitration slot should be used for
// msg with id virtual_ids[i]
uint8_t use_next_arbitration_window[3] = {0, 0, 0};
uint8_t number_of_node_ids;
// Number of slots and basic cycles in the system matrix.
// Is sent by the master node.
uint8_t slots_dyn = 0;
uint8_t basic_cycles_dyn = 0;
// A message used for initialization.
can_msg msg_to_send;
// Current basic cycle
uint8_t cycle_count = 0;
uint8_t cycle_count_next = 0;
// Current slot
uint8_t slot_count = 0;
// Flags.
uint8_t got_hb = 0;
uint8_t got_msg = 0;
uint8_t slot_used = 0;
uint8_t spi_in_use = 0;
void set_spi_in_use(uint8_t u) {
	spi_in_use = u;
}
// Checks if there was a heartbeat received
uint8_t got_heartbeat() {
	cli();
	if(got_hb) {
		got_hb = 0;
		sei();
		return 1;
	} else {
		sei();
		return 0;
	}
	
}
// Checks if there was a message received
uint8_t got_message() {
	if (got_msg) {
		got_msg = 0;
		return 1;
	} else {
		return 0;
	}
}
// Use the next available arbitration window to send a message with the
// id virtual_ids[i].
void set_arbitration(uint8_t i) {
	use_next_arbitration_window[i] = 1;
}
// Some getter&setter functions.
void set_cycle_count(uint8_t i) {
	cycle_count = i;
}
void set_slot_count(uint8_t i) {
	slot_count = i;
}
void set_node_id(uint16_t id, uint8_t i) {
	if ((i < 0) || (i > 2)) {
		return;
	}
	virtual_ids[i] = id;
}
uint16_t get_node_id(uint8_t i) {
	if (i > 2) {
		return 0;
	}
	return virtual_ids[i];
}
uint8_t get_current_basic_cycle() {
	return cycle_count;
}
uint8_t get_current_slot() {
	return slot_count;
}
// Initialize timer 1. Should trigger ISR after SLOTTIME.
void timer1_init() {
	// Initialize Timer1
	// Maximum counter value: 65535
	cli();          // disable global interrupts
	TCCR1A = 0;     // set entire TCCR1A register to 0
	TCCR1B = 0;     // same for TCCR1B
	
	// Compare match reached every SLOTTIME
	OCR1A = (SLOTTIME * 2);
	// turn on CTC mode (clear timer on compare match):
	TCCR1B |= (1 << WGM12);
	// Set CSnm bits for 1/8 prescaler -> 2 MHz set only CS11
	TCCR1B |= (1 << CS11);
	// Enable timer compare interrupt:
	TIMSK1 |= (1 << OCIE1A);
	// Enable global interrupts
	sei();
}
// Initialize Timer 2. This timer is used to get better synchronization
// when receiving a heartbeat.
void timer2_init() {
	TCCR2A = 0;
	TCCR2B = 0;
	// The prescaler MUST be the same as timer 1!
	// Set CSnm bits for 1/8 prescaler -> 2 MHz set only CS11
	TCCR2B |= (1 << CS11);
}
// Prepare the Start of Frame Interrupt. Only used by node_init().
void prepare_sof_interrupt() {
	cli();
	DDRE &= ~(1<<PE5);
	// React to a rising edge 0->1
	EICRB |= ((1<<ISC50) | (1<<ISC51));
	// Activate interrupt INT5
	EIMSK |= (1<<INT5);
	sei();
}
// Enable RX0BF Interrupt on Pin PJ1 and the RX1BF Interrupt on Pin PK0.
void prepare_rxbf_interrupt() {
	cli();
	PCICR = (1<<PCIE1) | (1<<PCIE2);
	PCMSK1 = (1<<PCINT10);
	PCMSK2 = (1<<PCINT16);
	PORTJ |= (1<<PJ1);
	sei();
}
// Build the node id. Is only used by node_init().
void build_id(uint8_t nodetype, uint8_t number, uint8_t recordme, uint8_t i) {
	virtual_ids[i] = (uint16_t) nodetype << 7 | recordme << 6 | number;
}
// MASTER ONLY
// Send a reference message.
void heartbeat() {
	// Send the prepared heartbeat.
	can_sendmsg_prepared(0);
	cycle_count = cycle_count_next;
	// Prepare the next heartbeat.
	cycle_count_next++;
	if (cycle_count_next == BASIC_CYCLES_HARD) {
		cycle_count_next = 0;
	}
	msg_to_send.data[0] = cycle_count_next;
	msg_to_send.length = 1;
	msg_to_send.id = virtual_ids[0];
	// Wait for the buffer to be ready again.
	// This takes 18,5µs, so it does not exceed the slot time.
	uint8_t temp = can_readreg(TXB0CTRL);
	while (temp & (1 << TXREQ)) {
		temp = can_readreg(TXB0CTRL);
	}
	can_preparemsg(msg_to_send, 0);
}
// MASTER ONLY
// Transmit the system matrix.
void transmit_matrix() {
	msg_to_send.id = virtual_ids[0];
	msg_to_send.length = 4;
	for (uint8_t i = 0; i < SLOTS_HARD * BASIC_CYCLES_HARD; i++) {
		msg_to_send.data[0] = i;
		msg_to_send.data[1] = msg_id[i];
		msg_to_send.data[2] = msg_id[i] >> 8;
		msg_to_send.data[3] = is_exclusive_window[i];
		can_preparemsg(msg_to_send, 0);
		can_sendmsg_prepared(0);
		_delay_ms(5);
	}
	msg_to_send.data[0] = SLOTS_HARD * BASIC_CYCLES_HARD;
	can_preparemsg(msg_to_send, 0);
	can_sendmsg_prepared(0);
	_delay_ms(1);
}
// SLAVE ONLY
// Receive the system matrix.
void receive_matrix() {
	int i = 0;
	while (1) {
		if (can_readreg(CANINTF) & 0x01) {
			can_getmsg0(&msg_to_send);
		}
		// Check for master id
		if (!(msg_to_send.id >> 7)) {
			i = msg_to_send.data[0];
			if (i == slots_dyn * basic_cycles_dyn) {
				return;
			}
			msg_id[i] = msg_to_send.data[1];
			msg_id[i] |= (uint16_t) (msg_to_send.data[2] << 8);
			is_exclusive_window[i] = msg_to_send.data[3];
		}
	}
}
// Set the acceptance Masks and filters so that heartbeats go to Receive Buffer 0 only.
// All other valid messages will go to Receive Buffer 1.
void set_masks_and_filters() {
	can_set_config_mode();
	_delay_ms(20);
	// Turn RXnBF pins on
	can_writereg(BFPCTRL, 0x0f);
	can_writereg(RXB0CTRL, 0x24);
	can_writereg(RXB1CTRL, 0x20);
	
	// Reset error flags
	can_writereg(EFLG, 0x00);
	can_writereg(CANINTF, 0x00);
	
	// Filter: Buffer 0 accepts only messages with a master id (starting wit 0000).
	// Buffer 1 accepts all valid messages with standard identifiers.
	can_writereg(RXF0SIDH, 0x00);
	can_writereg(RXF1SIDH, 0x00);
	// MASKS: Only Buffer 0 rejects any messages.
	can_writereg(RXM0SIDH, 0xF0);
	can_writereg(RXM0SIDL, 0x00);
	can_writereg(RXM1SIDH, 0x00);
	can_writereg(RXM1SIDL, 0x00);
	can_set_normal_mode(1);
}
// Checks if the ids of this node are in the current schedule. If not,
// they may be sent in an arbitration window.
void check_for_arbitration() {
	for (int i = 0; i < (slots_dyn * basic_cycles_dyn); i++) {
		for (int j = 0; j < number_of_node_ids; j++) {
			if (msg_id[i] == virtual_ids[j]) {
				arbitration_allowed[j] = 0;
			}
		}
	}
}
// Initialize the CAN node depending on its task.
// nodetype is a number between 0 and 15. See TTCAN.h for the different types.
// recordme is either 0 or 1. Set to 1 if node should be visible for a sniffer.
// number is the id of this node in its subgroup (nodetype), max: 31.
uint8_t node_init(uint8_t nodetype[], uint8_t number[], uint8_t recordme[], uint8_t number_of_ids) {
	if ((number_of_ids < 1) || (number_of_ids > 3)) {
		return 0;
	}
	can_init();
	
	_delay_ms(1);
	number_of_node_ids = number_of_ids;
	// Build the node ids.
	for (uint8_t i = 0; i < number_of_ids; i++) {
		build_id(nodetype[i], number[i], recordme[i], i);
	}
	if (nodetype[0] == MASTER) {
		can_set_normal_mode(1);
		_delay_ms(300);
		basic_cycles_dyn = BASIC_CYCLES_HARD;
		slots_dyn = SLOTS_HARD;
		// Make sure the master is slower than the slaves.
		_delay_ms(50);
		msg_to_send.id = virtual_ids[0];
		// Send the info how many basic cycles there are.
		msg_to_send.data[0] = CYCLE_INFO;
		msg_to_send.data[1] = BASIC_CYCLES_HARD;
		msg_to_send.data[2] = SLOTS_HARD;
		msg_to_send.length = 7;
		can_preparemsg(msg_to_send, 0);
		can_sendmsg_prepared(0);
		_delay_ms(50);
		// Tell the slaves to get ready for receiving the matrix.
		msg_to_send.data[0] = MATRIX;
		msg_to_send.length = 8;
		can_preparemsg(msg_to_send, 0);
		can_sendmsg_prepared(0);
		_delay_ms(10);
		// Transmit the matrix.
		transmit_matrix();
		set_masks_and_filters();
		
		//Prepare the first heartbeat.
		msg_to_send.data[0] = 0;
		msg_to_send.length = 1;
		
		can_preparemsg(msg_to_send, 0);
		prepare_rxbf_interrupt();
		check_for_arbitration();
		// Empty the message buffer.
		can_getmsg1(&msg_to_send);
		//Send the first heartbeat.
		heartbeat();
		_delay_ms(1);
		// Initialize Timer 1. From now on, every SLOTTIME there will be the
		// ISR(TIMER1_COMPA_vect).
		timer1_init();
	} else {
		for (uint8_t i = 0; i<128;i++) {
			is_exclusive_window[i] = 1;
		}
		// Wait for the system matrix.
		while (1) {
			if (can_readreg(CANINTF) & 0x01) {
				can_getmsg0(&msg_to_send);
			}
			if ((msg_to_send.data[0] == CYCLE_INFO) && (msg_to_send.length == 7) && !(msg_to_send.id >> 7)) {
				basic_cycles_dyn = msg_to_send.data[1];
				slots_dyn = msg_to_send.data[2];
			}
			if ((msg_to_send.data[0] == MATRIX) && (msg_to_send.length == 8) && !(msg_to_send.id >> 7)) {
				break;
			}
		}
		// Receive the system matrix.
		receive_matrix();
		set_masks_and_filters();
		prepare_rxbf_interrupt();
		check_for_arbitration();
		slot_count = slots_dyn;
		// Wait for hb
		while ((PINJ & (1 << PJ1)));
		timer1_init();
		timer2_init();
		prepare_sof_interrupt();
		// Empty the Heartbeat buffer!
		can_getmsg0(&msg_to_send);
		// Empty the message buffer.
		can_getmsg1(&msg_to_send);
	}
	return 0;
}
// Is transmit buffer buf safe to write to?
uint8_t buffer_ok_to_load(uint8_t buf) {
	if (spi_in_use) {
		return 0;
	}
	int pos = (cycle_count * slots_dyn) + slot_count;
	// If we are in a send slot for this buffer: no
	if (msg_id[pos] == virtual_ids[buf] || slot_used) {
		return 0;
	}
	// If this/next slot is a hb slot: no
	// If a hb or msg interrupt waits to get handled: no
	if (slot_count >= (slots_dyn - 1) || slot_count == 0 || (!(PINJ & (1 << PJ1))) || (!(PINK & (1<<PK0)))) {
		return 0;
	}
	// If the next slot is a send slot for this buffer and this slot is older than 50 µs: no
	if ((msg_id[pos+1] == virtual_ids[buf]) && (TCNT1 > 100)) {
		return 0;
	}
	return 1;
}
// Safely set the next message to send.
uint8_t ttcan_set_as_next_msg(can_msg msg, uint8_t bufno) {
	if (bufno < 0 || bufno > 3) {
		return 0;
	}
	if (buffer_ok_to_load(bufno)) {
		cli();
		load_buffer_flag[bufno] = 0;
		spi_in_use = 1;
		sei();
		can_preparemsg(msg, bufno);
		spi_in_use = 0;
		return 1;
	} else {
		messages_to_send[bufno] = msg;
		load_buffer_flag[bufno] = 1;
		return 0;
	}
}
// Read a msg from a buffer and avoid SPI mixups.
// bufno has to be 0 or 1. If bufno is not 0, receive buffer 1
// will be read.
void ttcan_getmsg(can_msg *msg, uint8_t bufno) {
	spi_in_use = 1;
	if (bufno) {
		can_getmsg1(msg);
	} else {
		can_getmsg0(msg);
	}
	spi_in_use = 0;
}
// Timer 1 interrupt, happens at the beginning of every slot.
ISR(TIMER1_COMPA_vect)
{
	// Uncomment to get a short pulse on Digital Pin 30 for timing observation.
	//PORTC |= (1 << PC7);
	slot_used = 0;
	
	#if ISMASTER
		
		slot_count++;
		if (slot_count == SLOTS_HARD) {
			slot_count = 0;
			heartbeat();
			slot_used = 1;
		}
		// If this is a window for you, then send your message.
		// Start at 1 bc 0 is always the master id.
		int temp = (cycle_count * slots_dyn) + slot_count;
		for (uint8_t i = 1; i < number_of_node_ids; i++) {
			if(msg_id[temp] == virtual_ids[i]) {
				can_sendmsg_prepared(i);
				slot_used = 1;
			}
		}
		
		// If this is an arbitration window, send something if you
		// can.
		if (!is_exclusive_window[temp] && (!slot_used)) {
			for (uint8_t i = 1; i  <number_of_node_ids; i++) {
				if (arbitration_allowed[i] && use_next_arbitration_window[i]) {
					can_sendmsg_prepared(i);
					slot_used = 1;
					//### Stop trying to send in case bus is busy
					_delay_us(3);
					can_bitmod(CANCTRL,0x10,0xff);
					can_bitmod(CANCTRL,0x10,0x00);
				}
			}
		}
	#else
	
		slot_count++;
		if (slot_count >= slots_dyn) {
			slot_count = slots_dyn - 1;
			//PORTC &= ~(1 << PC7);
			return;
		}
		// If this is a window for you, then send your message.
		int temp = (cycle_count * slots_dyn) + slot_count;
		for (uint8_t i = 0; i < number_of_node_ids; i++) {
			if(msg_id[temp] == virtual_ids[i]) {
				can_sendmsg_prepared(i);
				slot_used = 1;
			}
		}
		
		// If this is an arbitration window, send something if you
		// can.
		if (!is_exclusive_window[temp] && ((!slot_used) && (msg_id[temp] == 0))) {
			for (uint8_t i = 0; i<number_of_node_ids;i++) {
				if (arbitration_allowed[i] && use_next_arbitration_window[i]) {
					can_sendmsg_prepared(i);
					use_next_arbitration_window[i] = 0;
					slot_used = 1;
					//### Stop trying to send in case bus is busy
					if (is_exclusive_window[temp+1]) {
						_delay_us(3);
						can_bitmod(CANCTRL,0x10,0xff);
						can_bitmod(CANCTRL,0x10,0x00);
					}
				}
			}
		}
		
	#endif
	// If this is not a slot to send a msg, load a buffer.
	if (!slot_used) {
		for (int i = 0; i < number_of_node_ids; i++) {
			if (load_buffer_flag[i] && (!spi_in_use)) {
				can_preparemsg(messages_to_send[i], i);
				load_buffer_flag[i] = 0;
				break;
			}
		}
	}
	//PORTC &= ~(1 << PC7);
}
//The interrupt routine for the RX1BF pin PK0, i.e.,
// what happens if you receive a normal message.
ISR(PCINT2_vect){
	// Only react to a falling edge.
	if (!(PINK & (1<<PK0))) {
		got_msg = 1;
	}
}
// The interrupt routine for the RX0BF pin PJ1, i.e.,
// what happens if you receive a heartbeat.
ISR(PCINT1_vect){
	// Only react to a falling edge.
	if (!(PINJ & (1 << PJ1))) {
		TCNT1 = TCNT2;
		got_hb = 1;
		slot_count = 0;
		cycle_count++;
		if (cycle_count >= basic_cycles_dyn) {
			cycle_count = 0;
		}	
	}
}
// The Start of Frame pin is low.
ISR(INT5_vect) {
	TCNT2 = 0;
}