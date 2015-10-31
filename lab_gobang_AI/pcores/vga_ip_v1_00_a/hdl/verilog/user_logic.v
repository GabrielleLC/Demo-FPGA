A//----------------------------------------------------------------------------
// user_logic.vhd - module
//----------------------------------------------------------------------------
//
// ***************************************************************************
// ** Copyright (c) 1995-2011 Xilinx, Inc.  All rights reserved.            **
// **                                                                       **
// ** Xilinx, Inc.                                                          **
// ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
// ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
// ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
// ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
// ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
// ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
// ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
// ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
// ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
// ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
// ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
// ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
// ** FOR A PARTICULAR PURPOSE.                                             **
// **                                                                       **
// ***************************************************************************
//
//----------------------------------------------------------------------------
// Filename:          user_logic.vhd
// Version:           1.00.a
// Description:       User logic module.
// Date:              Sat Dec 01 11:20:05 2012 (by Create and Import Peripheral Wizard)
// Verilog Standard:  Verilog-2001
//----------------------------------------------------------------------------
// Naming Conventions:
//   active low signals:                    "*_n"
//   clock signals:                         "clk", "clk_div#", "clk_#x"
//   reset signals:                         "rst", "rst_n"
//   generics:                              "C_*"
//   user defined types:                    "*_TYPE"
//   state machine next state:              "*_ns"
//   state machine current state:           "*_cs"
//   combinatorial signals:                 "*_com"
//   pipelined or register delay signals:   "*_d#"
//   counter signals:                       "*cnt*"
//   clock enable signals:                  "*_ce"
//   internal version of output port:       "*_i"
//   device pins:                           "*_pin"
//   ports:                                 "- Names begin with Uppercase"
//   processes:                             "*_PROCESS"
//   component instantiations:              "<ENTITY_>I_<#|FUNC>"
//----------------------------------------------------------------------------

module user_logic
(
  // -- ADD USER PORTS BELOW THIS LINE ---------------
  // --USER ports added here 
  CLK_25M,
  LED,
  VGA_R,
  VGA_G,
  VGA_B,
  VGA_HSYNC,
  VGA_VSYNC,
  // -- ADD USER PORTS ABOVE THIS LINE ---------------

  // -- DO NOT EDIT BELOW THIS LINE ------------------
  // -- Bus protocol ports, do not add to or delete 
  Bus2IP_Clk,                     // Bus to IP clock
  Bus2IP_Reset,                   // Bus to IP reset
  Bus2IP_Data,                    // Bus to IP data bus
  Bus2IP_BE,                      // Bus to IP byte enables
  Bus2IP_RdCE,                    // Bus to IP read chip enable
  Bus2IP_WrCE,                    // Bus to IP write chip enable
  IP2Bus_Data,                    // IP to Bus data bus
  IP2Bus_RdAck,                   // IP to Bus read transfer acknowledgement
  IP2Bus_WrAck,                   // IP to Bus write transfer acknowledgement
  IP2Bus_Error                    // IP to Bus error response
  // -- DO NOT EDIT ABOVE THIS LINE ------------------
); // user_logic

// -- ADD USER PARAMETERS BELOW THIS LINE ------------
// --USER parameters added here 
parameter h_a = 96, h_b = 48, h_c = 640, h_d = 16, h_total= h_a + h_b + h_c + h_d;
parameter v_a = 2, v_b = 33, v_c = 480, v_d = 10, v_total = v_a + v_b + v_c + v_d;
parameter chesscolor1 = 8'b11111111;
parameter chesscolor2 = 8'b00000000;//8'b00000101;
parameter boardcolor  = 8'b10010101;
parameter cursorcolor = 8'b00100101;//8'b11100000;
parameter linecolor   = 8'b00000000;
// -- ADD USER PARAMETERS ABOVE THIS LINE ------------

// -- DO NOT EDIT BELOW THIS LINE --------------------
// -- Bus protocol parameters, do not add to or delete
parameter C_SLV_DWIDTH                   = 32;
parameter C_NUM_REG                      = 1;
// -- DO NOT EDIT ABOVE THIS LINE --------------------

// -- ADD USER PORTS BELOW THIS LINE -----------------
// --USER ports added here 
input                                     CLK_25M;
output     [0 : 7]                        LED;
output     [0 : 2]                        VGA_R;
output     [0 : 2]                        VGA_G;
output     [0 : 1]                        VGA_B;
output                                    VGA_HSYNC;
output                                    VGA_VSYNC;
// -- ADD USER PORTS ABOVE THIS LINE -----------------

// -- DO NOT EDIT BELOW THIS LINE --------------------
// -- Bus protocol ports, do not add to or delete
input                                     Bus2IP_Clk;
input                                     Bus2IP_Reset;
input      [0 : C_SLV_DWIDTH-1]           Bus2IP_Data;
input      [0 : C_SLV_DWIDTH/8-1]         Bus2IP_BE;
input      [0 : C_NUM_REG-1]              Bus2IP_RdCE;
input      [0 : C_NUM_REG-1]              Bus2IP_WrCE;
output     [0 : C_SLV_DWIDTH-1]           IP2Bus_Data;
output                                    IP2Bus_RdAck;
output                                    IP2Bus_WrAck;
output                                    IP2Bus_Error;
// -- DO NOT EDIT ABOVE THIS LINE --------------------

//----------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------

  // --USER nets declarations added here, as needed for user logic 
  reg        [0 : 9]                        h_cnt;
  reg        [0 : 9]                        v_cnt;
  reg        [0 : 0]                        h_sync;
  reg        [0 : 0]                        v_sync;
  reg        [0 : 0]                        vga_ena;
  reg        [0 : 7]                        vga_data;
  reg        [0 : 9]                        x_cor;
  reg        [0 : 9]                        y_cor;
  reg        [0 : 1]                        board[0 : 225-1];
  reg        [0 : 1]                        menu[0 : 300-1];
  reg        [0 : 7]                        colormap[0 : 3];
  integer                                   i;
  // Nets for user logic slave model s/w accessible register example
  reg        [0 : C_SLV_DWIDTH-1]           slv_reg0;
  wire       [0 : 0]                        slv_reg_write_sel;
  wire       [0 : 0]                        slv_reg_read_sel;
  reg        [0 : C_SLV_DWIDTH-1]           slv_ip2bus_data;
  wire                                      slv_read_ack;
  wire                                      slv_write_ack;
  integer                                   byte_index, bit_index;

  // --USER logic implementation added here
  initial
  begin
    colormap[0] = chesscolor1;
	 colormap[1] = chesscolor2;
	 colormap[2] = boardcolor;
	 colormap[3] = cursorcolor;
  end
  
  always @( posedge Bus2IP_Clk )
  begin
    if ( Bus2IP_Reset == 1 )
	   begin
		  for (i = 0; i < 225; i=i+1)
		    board[i] = 2'b10;
		  for (i = 0; i < 300; i=i+1)
		    menu[i] = 2'b10;
		end
	 else
	   begin
        if ( Bus2IP_WrCE == 1'b1)
		  begin
		    if (Bus2IP_Data[16 : 23]== 0)
			     board[Bus2IP_Data[0 : 7] + Bus2IP_Data[8 : 15] * 15] = Bus2IP_Data[24 : 31];
			 else if (Bus2IP_Data[16 : 23]== 1)
			     menu[Bus2IP_Data[0 : 7] + Bus2IP_Data[8 : 15] * 10] = Bus2IP_Data[24 : 31];
			 end
		end
  end
  
  always @( posedge CLK_25M )
  begin
    if (Bus2IP_Reset == 1)
	   begin
		  h_cnt <= 0;
		  v_cnt <= 0;
		end
	 else
	   begin
        if (v_cnt == v_total-1)
		    v_cnt <= 0;
		  if (h_cnt == h_total-1)
			 begin 
		      h_cnt <= 0;
				v_cnt <= v_cnt+1;
			 end
		  else
			 h_cnt <= h_cnt+1;
      end
  end
  
  always@(*)
  begin
    if (h_cnt < h_a)
	   h_sync <= 0;
	 else		     
		h_sync <= 1;
	 if (v_cnt < v_a)
		v_sync <= 0;
	 else
		v_sync <= 1;	
    if ((h_cnt >= h_a + h_b && h_cnt < h_a + h_b + h_c) && (v_cnt >= v_a + v_b && v_cnt < v_a + v_b + v_c))
	   begin
		  vga_ena <= 1;
		  x_cor <= h_cnt - h_a - h_b;
		  y_cor <= v_cnt - v_a - v_b;
		end
    else 
		vga_ena <= 0;	  
  end

    always@(*)
  begin
    if (vga_ena == 1)
	   begin
		  if (x_cor < 481)
		    begin
		  if((x_cor%32-16)*(x_cor%32-16)+(y_cor%32-16)*(y_cor%32-16)<196 && board[x_cor / 32 + y_cor / 32 * 15]!=2)
		    vga_data <= colormap[board[x_cor / 32 + y_cor / 32 * 15]]; 
		  else if(x_cor[5 : 9]==5'b10010 || y_cor[5 : 9]==5'b10010 || x_cor==480)
		    vga_data <= linecolor;
		  else vga_data <= 8'b11110101;;
      end
		  else
		      vga_data <= colormap[menu[(x_cor - 480) / 16 + y_cor / 16 * 10]];
		end
	 else
		vga_data <= 0;
  end
  /////////////////////////////////////////////////////////////////////////
  assign LED = 0;
  assign VGA_R = vga_data[0 : 2];
  assign VGA_G = vga_data[3 : 5];
  assign VGA_B = vga_data[6 : 7];
  assign VGA_HSYNC = h_sync;
  assign VGA_VSYNC = v_sync;

  // ------------------------------------------------------
  // Example code to read/write user logic slave model s/w accessible registers
  // 
  // Note:
  // The example code presented here is to show you one way of reading/writing
  // software accessible registers implemented in the user logic slave model.
  // Each bit of the Bus2IP_WrCE/Bus2IP_RdCE signals is configured to correspond
  // to one software accessible register by the top level template. For example,
  // if you have four 32 bit software accessible registers in the user logic,
  // you are basically operating on the following memory mapped registers:
  // 
  //    Bus2IP_WrCE/Bus2IP_RdCE   Memory Mapped Register
  //                     "1000"   C_BASEADDR + 0x0
  //                     "0100"   C_BASEADDR + 0x4
  //                     "0010"   C_BASEADDR + 0x8
  //                     "0001"   C_BASEADDR + 0xC
  // 
  // ------------------------------------------------------

  assign
    slv_reg_write_sel = Bus2IP_WrCE[0:0],
    slv_reg_read_sel  = Bus2IP_RdCE[0:0],
    slv_write_ack     = Bus2IP_WrCE[0],
    slv_read_ack      = Bus2IP_RdCE[0];

  // implement slave model register(s)
  always @( posedge Bus2IP_Clk )
    begin: SLAVE_REG_WRITE_PROC

      if ( Bus2IP_Reset == 1 )
        begin
          slv_reg0 <= 0;
        end
      else
        case ( slv_reg_write_sel )
          1'b1 :
            for ( byte_index = 0; byte_index <= (C_SLV_DWIDTH/8)-1; byte_index = byte_index+1 )
              if ( Bus2IP_BE[byte_index] == 1 )
                for ( bit_index = byte_index*8; bit_index <= byte_index*8+7; bit_index = bit_index+1 )
                  slv_reg0[bit_index] <= Bus2IP_Data[bit_index];
          default : ;
        endcase

    end // SLAVE_REG_WRITE_PROC

  // implement slave model register read mux
  always @( slv_reg_read_sel or slv_reg0 )
    begin: SLAVE_REG_READ_PROC

      case ( slv_reg_read_sel )
        1'b1 : slv_ip2bus_data <= slv_reg0;
        default : slv_ip2bus_data <= 0;
      endcase

    end // SLAVE_REG_READ_PROC

  // ------------------------------------------------------------
  // Example code to drive IP to Bus signals
  // ------------------------------------------------------------

  assign IP2Bus_Data    = slv_ip2bus_data;
  assign IP2Bus_WrAck   = slv_write_ack;
  assign IP2Bus_RdAck   = slv_read_ack;
  assign IP2Bus_Error   = 0;

endmodule
