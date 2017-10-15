//=======================================================
// ECE3400 Fall 2017
// Lab 3: Template top-level module
//
// Top-level skeleton from Terasic
// Modified by Claire Chen for ECE3400 Fall 2017
//=======================================================

`define ONE_SEC 50000000

module DE0_NANO(

	CLOCK_50,

	GPIO_0_D,
	GPIO_0_IN,

	GPIO_1_D,
	GPIO_1_IN,
);

	 localparam ONE_SEC = 50000000; // one second in 25MHz clock cycles

	 input 		          		CLOCK_50;

	 inout 		    [33:0]		GPIO_0_D;
	 input 		     [1:0]		GPIO_0_IN;

	 inout 		    [33:0]		GPIO_1_D;
	 input 		     [1:0]		GPIO_1_IN;
	 
	 wire [7:0]  DAC; //8-bit output to DAC
	 
	 reg [7:0] addr;
	 
	 //Wired up ROM table
	 rom Table (
		.clock(CLOCK_50),
		.addr(addr),
		.sine(DAC)
	 );
	
	 // Local parameters for notes
	localparam CLKDIVIDERA = ONE_SEC/(256*440);
	localparam CLKDIVIDERCsh = ONE_SEC/(256*554);
	localparam CLKDIVIDERE = ONE_SEC/(256*660);
	
	assign GPIO_0_D[7:0] = DAC;

	reg [15:0] counter; //counts cycles until moving to next ROM address
	reg [25:0] note_length; //counts cycles until moving to next note
	reg [1:0] note; //note being played. 01 is A, 10 is C sharp, 11 is E
	
	always @ (posedge CLOCK_50) begin
	
		//State machine switching between notes every second
		if(note_length == 0) begin
			note_length <= ONE_SEC;
			note <= note + 1;
		end
		
		else begin
			note_length <= note_length - 1;
		end
		
		
	  /*State machine to output ROM sin value after
	  a specified sin period based on note*/
	  if (counter == 0) begin
			case(note)
				2'b00: counter    <= ONE_SEC;
				2'b01: counter    <= CLKDIVIDERA - 1;
				2'b10: counter    <= CLKDIVIDERCsh - 1;
				2'b11: counter    <= CLKDIVIDERE - 1;	
			endcase
			
			if (addr < 256) begin
				addr       <= addr+1;
			end
			else begin
				addr <= 0;
			end	 
	  end
		  
	 else begin
			counter    <= counter - 1;
	 end
	 
	end	
	
endmodule


//ROM module containing sin wave values for one period
module rom(clock,addr,sine);
 input clock;
 input [7:0] addr;
 output [7:0] sine;
 reg    [7:0] sine;
 always@(posedge clock) begin
   case(addr)
	   8'd0: sine = 8'd255;
		8'd1: sine = 8'd255;
		8'd2: sine = 8'd255;
		8'd3: sine = 8'd255;
		8'd4: sine = 8'd254;
		8'd5: sine = 8'd254;
		8'd6: sine = 8'd254;
		8'd7: sine = 8'd253;
		8'd8: sine = 8'd253;
		8'd9: sine = 8'd252;
		8'd10: sine = 8'd251;
		8'd11: sine = 8'd250;
		8'd12: sine = 8'd250;
		8'd13: sine = 8'd249;
		8'd14: sine = 8'd248;
		8'd15: sine = 8'd246;
		8'd16: sine = 8'd245;
		8'd17: sine = 8'd244;
		8'd18: sine = 8'd243;
		8'd19: sine = 8'd241;
		8'd20: sine = 8'd240;
		8'd21: sine = 8'd238;
		8'd22: sine = 8'd237;
		8'd23: sine = 8'd235;
		8'd24: sine = 8'd234;
		8'd25: sine = 8'd232;
		8'd26: sine = 8'd230;
		8'd27: sine = 8'd228;
		8'd28: sine = 8'd226;
		8'd29: sine = 8'd224;
		8'd30: sine = 8'd222;
		8'd31: sine = 8'd220;
		8'd32: sine = 8'd218;
		8'd33: sine = 8'd215;
		8'd34: sine = 8'd213;
		8'd35: sine = 8'd211;
		8'd36: sine = 8'd208;
		8'd37: sine = 8'd206;
		8'd38: sine = 8'd203;
		8'd39: sine = 8'd201;
		8'd40: sine = 8'd198;
		8'd41: sine = 8'd196;
		8'd42: sine = 8'd193;
		8'd43: sine = 8'd190;
		8'd44: sine = 8'd188;
		8'd45: sine = 8'd185;
		8'd46: sine = 8'd182;
		8'd47: sine = 8'd179;
		8'd48: sine = 8'd176;
		8'd49: sine = 8'd173;
		8'd50: sine = 8'd170;
		8'd51: sine = 8'd167;
		8'd52: sine = 8'd165;
		8'd53: sine = 8'd162;
		8'd54: sine = 8'd158;
		8'd55: sine = 8'd155;
		8'd56: sine = 8'd152;
		8'd57: sine = 8'd149;
		8'd58: sine = 8'd146;
		8'd59: sine = 8'd143;
		8'd60: sine = 8'd140;
		8'd61: sine = 8'd137;
		8'd62: sine = 8'd134;
		8'd63: sine = 8'd131;
		8'd64: sine = 8'd128;
		8'd65: sine = 8'd124;
		8'd66: sine = 8'd121;
		8'd67: sine = 8'd118;
		8'd68: sine = 8'd115;
		8'd69: sine = 8'd112;
		8'd70: sine = 8'd109;
		8'd71: sine = 8'd106;
		8'd72: sine = 8'd103;
		8'd73: sine = 8'd100;
		8'd74: sine = 8'd97;
		8'd75: sine = 8'd93;
		8'd76: sine = 8'd90;
		8'd77: sine = 8'd88;
		8'd78: sine = 8'd85;
		8'd79: sine = 8'd82;
		8'd80: sine = 8'd79;
		8'd81: sine = 8'd76;
		8'd82: sine = 8'd73;
		8'd83: sine = 8'd70;
		8'd84: sine = 8'd67;
		8'd85: sine = 8'd65;
		8'd86: sine = 8'd62;
		8'd87: sine = 8'd59;
		8'd88: sine = 8'd57;
		8'd89: sine = 8'd54;
		8'd90: sine = 8'd52;
		8'd91: sine = 8'd49;
		8'd92: sine = 8'd47;
		8'd93: sine = 8'd44;
		8'd94: sine = 8'd42;
		8'd95: sine = 8'd40;
		8'd96: sine = 8'd37;
		8'd97: sine = 8'd35;
		8'd98: sine = 8'd33;
		8'd99: sine = 8'd31;
		8'd100: sine = 8'd29;
		8'd101: sine = 8'd27;
		8'd102: sine = 8'd25;
		8'd103: sine = 8'd23;
		8'd104: sine = 8'd21;
		8'd105: sine = 8'd20;
		8'd106: sine = 8'd18;
		8'd107: sine = 8'd17;
		8'd108: sine = 8'd15;
		8'd109: sine = 8'd14;
		8'd110: sine = 8'd12;
		8'd111: sine = 8'd11;
		8'd112: sine = 8'd10;
		8'd113: sine = 8'd9;
		8'd114: sine = 8'd7;
		8'd115: sine = 8'd6;
		8'd116: sine = 8'd5;
		8'd117: sine = 8'd5;
		8'd118: sine = 8'd4;
		8'd119: sine = 8'd3;
		8'd120: sine = 8'd2;
		8'd121: sine = 8'd2;
		8'd122: sine = 8'd1;
		8'd123: sine = 8'd1;
		8'd124: sine = 8'd1;
		8'd125: sine = 8'd0;
		8'd126: sine = 8'd0;
		8'd127: sine = 8'd0;
		8'd128: sine = 8'd0;
		8'd129: sine = 8'd0;
		8'd130: sine = 8'd0;
		8'd131: sine = 8'd0;
		8'd132: sine = 8'd1;
		8'd133: sine = 8'd1;
		8'd134: sine = 8'd1;
		8'd135: sine = 8'd2;
		8'd136: sine = 8'd2;
		8'd137: sine = 8'd3;
		8'd138: sine = 8'd4;
		8'd139: sine = 8'd5;
		8'd140: sine = 8'd5;
		8'd141: sine = 8'd6;
		8'd142: sine = 8'd7;
		8'd143: sine = 8'd9;
		8'd144: sine = 8'd10;
		8'd145: sine = 8'd11;
		8'd146: sine = 8'd12;
		8'd147: sine = 8'd14;
		8'd148: sine = 8'd15;
		8'd149: sine = 8'd17;
		8'd150: sine = 8'd18;
		8'd151: sine = 8'd20;
		8'd152: sine = 8'd21;
		8'd153: sine = 8'd23;
		8'd154: sine = 8'd25;
		8'd155: sine = 8'd27;
		8'd156: sine = 8'd29;
		8'd157: sine = 8'd31;
		8'd158: sine = 8'd33;
		8'd159: sine = 8'd35;
		8'd160: sine = 8'd37;
		8'd161: sine = 8'd40;
		8'd162: sine = 8'd42;
		8'd163: sine = 8'd44;
		8'd164: sine = 8'd47;
		8'd165: sine = 8'd49;
		8'd166: sine = 8'd52;
		8'd167: sine = 8'd54;
		8'd168: sine = 8'd57;
		8'd169: sine = 8'd59;
		8'd170: sine = 8'd62;
		8'd171: sine = 8'd65;
		8'd172: sine = 8'd67;
		8'd173: sine = 8'd70;
		8'd174: sine = 8'd73;
		8'd175: sine = 8'd76;
		8'd176: sine = 8'd79;
		8'd177: sine = 8'd82;
		8'd178: sine = 8'd85;
		8'd179: sine = 8'd88;
		8'd180: sine = 8'd90;
		8'd181: sine = 8'd93;
		8'd182: sine = 8'd97;
		8'd183: sine = 8'd100;
		8'd184: sine = 8'd103;
		8'd185: sine = 8'd106;
		8'd186: sine = 8'd109;
		8'd187: sine = 8'd112;
		8'd188: sine = 8'd115;
		8'd189: sine = 8'd118;
		8'd190: sine = 8'd121;
		8'd191: sine = 8'd124;
		8'd192: sine = 8'd127;
		8'd193: sine = 8'd131;
		8'd194: sine = 8'd134;
		8'd195: sine = 8'd137;
		8'd196: sine = 8'd140;
		8'd197: sine = 8'd143;
		8'd198: sine = 8'd146;
		8'd199: sine = 8'd149;
		8'd200: sine = 8'd152;
		8'd201: sine = 8'd155;
		8'd202: sine = 8'd158;
		8'd203: sine = 8'd162;
		8'd204: sine = 8'd165;
		8'd205: sine = 8'd167;
		8'd206: sine = 8'd170;
		8'd207: sine = 8'd173;
		8'd208: sine = 8'd176;
		8'd209: sine = 8'd179;
		8'd210: sine = 8'd182;
		8'd211: sine = 8'd185;
		8'd212: sine = 8'd188;
		8'd213: sine = 8'd190;
		8'd214: sine = 8'd193;
		8'd215: sine = 8'd196;
		8'd216: sine = 8'd198;
		8'd217: sine = 8'd201;
		8'd218: sine = 8'd203;
		8'd219: sine = 8'd206;
		8'd220: sine = 8'd208;
		8'd221: sine = 8'd211;
		8'd222: sine = 8'd213;
		8'd223: sine = 8'd215;
		8'd224: sine = 8'd218;
		8'd225: sine = 8'd220;
		8'd226: sine = 8'd222;
		8'd227: sine = 8'd224;
		8'd228: sine = 8'd226;
		8'd229: sine = 8'd228;
		8'd230: sine = 8'd230;
		8'd231: sine = 8'd232;
		8'd232: sine = 8'd234;
		8'd233: sine = 8'd235;
		8'd234: sine = 8'd237;
		8'd235: sine = 8'd238;
		8'd236: sine = 8'd240;
		8'd237: sine = 8'd241;
		8'd238: sine = 8'd243;
		8'd239: sine = 8'd244;
		8'd240: sine = 8'd245;
		8'd241: sine = 8'd246;
		8'd242: sine = 8'd248;
		8'd243: sine = 8'd249;
		8'd244: sine = 8'd250;
		8'd245: sine = 8'd250;
		8'd246: sine = 8'd251;
		8'd247: sine = 8'd252;
		8'd248: sine = 8'd253;
		8'd249: sine = 8'd253;
		8'd250: sine = 8'd254;
		8'd251: sine = 8'd254;
		8'd252: sine = 8'd254;
		8'd253: sine = 8'd255;
		8'd254: sine = 8'd255;
		8'd255: sine = 8'd255;
	endcase
 end
endmodule

