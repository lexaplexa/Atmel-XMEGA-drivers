/*************************************************************************
 * crc.h
 *
 * Created: 16.7.2015 13:04:31
 * Revised: 15.10.2015
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 * Cyclic redundancy check
 * -----------------------------------------------------------------------
 *
 * Example for CRC 8bit:
 * Input byte 0x57
 * Generator polynomial 0x07
 *
 *            0 1 0 1 0 1 1 1 (0x57 => Remainder)
 * Loop = 1 ------------------------------------------------
 *          0|1 0 1 0 1 1 1 0 (0xAE => Remainder shift left)
 * Loop = 2 ------------------------------------------------
 *          1|0 1 0 1 1 1 0 0 (0x5A => Remainder shift left)
 *          ^ 0 0 0 0 0 1 1 1 (0x07 => XOR)
 *          -----------------
 *            0 1 0 1 1 0 1 1 (0x5B => Remainder)
 * Loop = 3 ------------------------------------------------
 *          0|1 0 1 1 0 1 1 0 (0xB6 => Remainder shift left)
 * Loop = 4 ------------------------------------------------
 *          1|0 1 1 0 1 1 0 0 (0x6C => Remainder shift left)
 *          ^ 0 0 0 0 0 1 1 1 (0x07 => XOR)
 *          -----------------
 *            0 1 1 0 1 0 1 1 (0x6B => Remainder)
 * Loop = 5 ------------------------------------------------
 *          0|1 1 0 1 0 1 1 0 (0xD6 => Remainder shift left)
 * Loop = 6 ------------------------------------------------
 *          1|1 0 1 0 1 1 0 0 (0xAC => Remainder shift left)
 *          ^ 0 0 0 0 0 1 1 1 (0x07 => XOR)
 *          -----------------
 *            1 0 1 0 1 0 1 1 (0xAB => Remainder)
 * Loop = 7 ------------------------------------------------
 *          1|0 1 0 1 0 1 1 0 (0x56 => Remainder shift left)
 *          ^ 0 0 0 0 0 1 1 1 (0x07 => XOR)
 *          -----------------
 *            0 1 0 1 0 0 0 1 (0x51 => Remainder)
 * Loop = 8 ------------------------------------------------
 *          0|1 0 1 0 0 0 1 0 (0xA2 => Remainder shift left)
 *          return 0xA2;
 *
 *************************************************************************/


#ifndef CRC_H_
#define CRC_H_

/**
 * \brief   Generate CRC 8bit
 * 
 * \param unByte        - byte
 * \param unGenPol      - Generator polynomial
 * 
 * \return uint8_t
 */
uint8_t crc8(uint8_t unByte, uint8_t unGenPol);


/**
 * \brief   Generate CRC 8bit
 * 
 * \param aByteArray    - pointer to byte array
 * \param unLen         - data length
 * \param unGenPol      - Generator polynomial
 * 
 * \return uint8_t
 */
uint8_t crc8(uint8_t *aByteArray, uint16_t unLen, uint8_t unGenPol);


/**
 * \brief   Generate CRC for CAN messages
 * 
 * \param aByteArray    - pointer to byte array
 * \param unLen         - data length
 * \param unSafetyCode  - safety code added to checksum (defined by customer)
 * 
 * \return uint8_t
 */
uint8_t SPDUChechsum(uint8_t *aByteArray, uint16_t unLen, const uint8_t unSafetyCode);


/**
 * \brief   Generate CRC for CAN messages
 * 
 * \param aByteArray    - pointer to byte array
 * \param unLen         - data length
 * \param aSafetyCodes  - pointer to codes. One code added to checksum (defined by customer)
 * 
 * \return uint8_t
 */
uint8_t SPDUChechsum(uint8_t *aByteArray, uint16_t unLen, const uint8_t *aSafetyCodes);


/**
 * \brief   Generate CRC 16bit
 * 
 * \param unByte        - byte
 * \param unGenPol      - Generator polynomial
 * 
 * \return uint8_t
 */
uint16_t crc16(uint8_t unByte, uint16_t unGenPol);


/**
 * \brief   Generate CRC 16bit
 * 
 * \param aByteArray    - pointer to byte array
 * \param unLen         - data length
 * \param unGenPol      - Generator polynomial
 * 
 * \return uint8_t
 */
uint16_t crc16(uint8_t *aByteArray, uint16_t unLen, uint16_t unGenPol);

#endif /* INCFILE1_H_ */