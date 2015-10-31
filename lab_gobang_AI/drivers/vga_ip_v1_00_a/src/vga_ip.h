/*****************************************************************************
* Filename:          E:\Xilinx\EDKprojects\lab_vga/drivers/vga_ip_v1_00_a/src/vga_ip.h
* Version:           1.00.a
* Description:       vga_ip Driver Header File
* Date:              Sat Dec 01 11:20:07 2012 (by Create and Import Peripheral Wizard)
*****************************************************************************/

#ifndef VGA_IP_H
#define VGA_IP_H

/***************************** Include Files *******************************/

#include "xbasic_types.h"
#include "xstatus.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/


/**
 * User Logic Slave Space Offsets
 * -- SLV_REG0 : user logic slave module register 0
 */
#define VGA_IP_USER_SLV_SPACE_OFFSET (0x00000000)
#define VGA_IP_SLV_REG0_OFFSET (VGA_IP_USER_SLV_SPACE_OFFSET + 0x00000000)

/**************************** Type Definitions *****************************/


/***************** Macros (Inline Functions) Definitions *******************/

/**
 *
 * Write a value to a VGA_IP register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the VGA_IP device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void VGA_IP_mWriteReg(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Data)
 *
 */
#define VGA_IP_mWriteReg(BaseAddress, RegOffset, Data) \
 	Xil_Out32((BaseAddress) + (RegOffset), (Xuint32)(Data))

/**
 *
 * Read a value from a VGA_IP register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the VGA_IP device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 VGA_IP_mReadReg(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define VGA_IP_mReadReg(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (RegOffset))


/**
 *
 * Write/Read 32 bit value to/from VGA_IP user logic slave registers.
 *
 * @param   BaseAddress is the base address of the VGA_IP device.
 * @param   RegOffset is the offset from the slave register to write to or read from.
 * @param   Value is the data written to the register.
 *
 * @return  Data is the data from the user logic slave register.
 *
 * @note
 * C-style signature:
 * 	void VGA_IP_mWriteSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Value)
 * 	Xuint32 VGA_IP_mReadSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define VGA_IP_mWriteSlaveReg0(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_IP_SLV_REG0_OFFSET) + (RegOffset), (Xuint32)(Value))

#define VGA_IP_mReadSlaveReg0(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_IP_SLV_REG0_OFFSET) + (RegOffset))

/************************** Function Prototypes ****************************/


/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the VGA_IP instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus VGA_IP_SelfTest(void * baseaddr_p);

#endif /** VGA_IP_H */
