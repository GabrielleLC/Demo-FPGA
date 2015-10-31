##############################################################################
## Filename:          E:\Xilinx\EDKprojects\lab_vga/drivers/vga_ip_v1_00_a/data/vga_ip_v2_1_0.tcl
## Description:       Microprocess Driver Command (tcl)
## Date:              Sat Dec 01 11:20:07 2012 (by Create and Import Peripheral Wizard)
##############################################################################

#uses "xillib.tcl"

proc generate {drv_handle} {
  xdefine_include_file $drv_handle "xparameters.h" "vga_ip" "NUM_INSTANCES" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" 
}
