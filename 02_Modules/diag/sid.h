/*******************************************************
 * sid.h
 *
 * Created: 13.2.2014 11:26:45
 * Revised: 14.1.2015
 * Author: LeXa
 * ABOUT:
 * List of SIDs
 *******************************************************/ 


#ifndef SID_H_
#define SID_H_

typedef enum {
    SID_SESSION_CONTROL = 0x10,
    SID_RESET = 0x11,
    SID_READ_DATA_BY_ID = 0x22,
    SID_READ_MEMORY_BY_ADDRESS = 0x23,
    SID_WRITE_DATA_BY_ID = 0x2E,
    SID_WRITE_MEMORY_BY_ADDRESS = 0x3D,
    SID_TESTER_PRESENT = 0x3E,
    SID_NEGATIVE_RESPONSE = 0x7F,
}SID_t;

#endif /* SID_H_ */