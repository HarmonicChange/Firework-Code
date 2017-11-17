//=======================================================
// ECE3400 Fall 2017
// Written by Team 12
//=======================================================

`define ONE_SEC 25000000

module DE0_NANO(
    CLOCK_50,
    LED,
    KEY,
    SW,
    GPIO_0_D,
    GPIO_0_IN,
    GPIO_1_D,
    GPIO_1_IN,
);

     //=======================================================
     //  PARAMETER declarations
     //=======================================================

     localparam ONE_SEC = 25000000; // one second in 25MHz clock cycles
     localparam block_width = 10'd80;
     
     localparam black = 8'b000_000_00;
     localparam red   = 8'b111_000_00;
     localparam green = 8'b000_111_00;
     localparam blue  = 8'b000_000_11;
     localparam white = 8'b111_111_11;

     localparam wall  = 8'b111_010_00;
     localparam Tr07  = 8'b000_000_01; // Dark blue
     localparam Tr12  = 8'b000_111_11; // light blue
     localparam Tr17  = 8'b111_111_00; // Yellow

     localparam up    = 8'b000_010_00;
     localparam do    = 8'b000_001_00;
     localparam le    = 8'b000_000_10;
     localparam ri    = 8'b000_000_01;
         
     //=======================================================
     //  PORT declarations
     //=======================================================

     input                      CLOCK_50;
     output          [7:0]      LED;
     input           [1:0]      KEY;
     input           [3:0]      SW;
     inout          [33:0]      GPIO_0_D;
     input           [1:0]      GPIO_0_IN;
     inout          [33:0]      GPIO_1_D;
     input           [1:0]      GPIO_1_IN;

    //=======================================================
    //  REG/WIRE declarations
    //=======================================================
    reg         CLOCK_25;
    wire        reset;              // active high reset signal 

    wire [9:0]  PX_X;      // current x-coord from VGA driver
    wire [9:0]  PX_Y;      // current y-coord from VGA driver
    reg  [7:0]  PX_color;        // input 8-bit pixel color for current coords
     
    reg [24:0]  led_counter;        // timer to keep track of when to toggle LED
    reg              led_state;     // 1 is on, 0 is off
     
    // Module outputs coordinates of next pixel to be written onto screen
    VGA_DRIVER driver(
        .RESET(reset),
        .CLOCK(CLOCK_25),
        .PIXEL_COLOR_IN(PX_color),
        .PIXEL_X(PX_X),
        .PIXEL_Y(PX_Y),
        .PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
        .H_SYNC_NEG(GPIO_0_D[7]),
        .V_SYNC_NEG(GPIO_0_D[5])
    );
     
    assign reset = ~KEY[0]; // reset when KEY0 is pressed
         
    //=======================================================
    //  Structural coding
    //=======================================================
 
    // Generate 25MHz clock for VGA, FPGA has 50 MHz clock
    always @ (posedge CLOCK_50) begin
        CLOCK_25 <= ~CLOCK_25; 
    end // always @ (posedge CLOCK_50)
    
     
     //==============================
     // State Encoding
     //==============================
     
     reg [7:0] grid[0:19];
     reg [7:0] grid_color[0:19];
     
     //Chip Select
     reg currentPackage;
     reg previousPackage;
     
     //SPI Data
     reg SPIData;
     
     //SPI Clock
     reg currSPIClock;
     reg prevSPIClock;
         
     reg packageSending;
     
      // Store SPI Data
     reg [7:0] positionData;
     reg [5:0] currentLocation;
      reg [1:0] currentOrientation;
     
     //Number of bits yet to be received
     reg [2:0] currDataBits;
      
    initial begin
        grid_color[0] = 8'b000_0_0000;
        grid_color[1] = 8'b000_0_0000;
        grid_color[2] = 8'b000_0_0000;
        grid_color[3] = 8'b000_0_0000;
        grid_color[4] = 8'b000_0_0000;
        grid_color[5] = 8'b000_0_0000;
        grid_color[6] = 8'b000_0_0000;
        grid_color[7] = 8'b000_0_0000;
        grid_color[8] = 8'b000_0_0000;
        grid_color[9] = 8'b000_0_0000;
        grid_color[10] = 8'b000_0_0000;
        grid_color[11] = 8'b000_0_0000;
        grid_color[12] = 8'b000_0_0000;
        grid_color[13] = 8'b000_0_0000;
        grid_color[14] = 8'b000_0_0000;
        grid_color[15] = 8'b000_0_0000;
        grid_color[16] = 8'b000_0_0000;
        grid_color[17] = 8'b000_0_0000;
        grid_color[18] = 8'b000_0_0000;
        grid_color[19] = 8'b000_0_0000;
    end
     
     //SPI receiving
        // Should always receive 1 byte for robot location data, then 25 bytes with maze info
     always @ (posedge CLOCK_50) begin
            // GPIO Pin 33 is ChipSelect
            
            currentPackage <= GPIO_0_D[33];

            // Begin data transmission
            if (currentPackage == 1'b1 && previousPackage == 1'b0) begin  
                packageSending <= 1'b1;          // Starts package transmission
                currDataBits <= 3'd7;            // Start receiving bits from the highest bit                
            end

            // End data transmission
            else if (currentPackage == 1'b0 && previousPackage == 1'b1) begin  //Falling edge of CS
                packageSending <= 1'b0;          // Ends package transmission

                if (positionData[7] == 1'b1) begin      // If MSB == 1, then this is maze info
                    
                    case(positionData[3:0])
                        4'b0000: grid_color[currentLocation][4:0] <= 5'b100_00;             
                        4'b0001: grid_color[currentLocation][4:0] <= 5'b100_01;                           
                        4'b0010: grid_color[currentLocation][4:0] <= 5'b100_10;                
                        4'b0011: grid_color[currentLocation][4:0] <= 5'b100_11;
                                
                        4'b0100: grid_color[currentLocation][4:0] <= 5'b101_00;     
                        4'b0101: grid_color[currentLocation][4:0] <= 5'b101_01;               
                        4'b0110: grid_color[currentLocation][4:0] <= 5'b101_10;                  
                        4'b0111: grid_color[currentLocation][4:0] <= 5'b101_11; 
                                
                        4'b1000: grid_color[currentLocation][4:0] <= 5'b110_00;    
                        4'b1001: grid_color[currentLocation][4:0] <= 5'b110_01;  
                        4'b1010: grid_color[currentLocation][4:0] <= 5'b110_10;  
                        4'b1011: grid_color[currentLocation][4:0] <= 5'b110_11; 
                                
                        4'b1100: grid_color[currentLocation][4:0] <= 5'b111_00;  
                        4'b1101: grid_color[currentLocation][4:0] <= 5'b111_01;   
                        4'b1110: grid_color[currentLocation][4:0] <= 5'b111_10;   
                        4'b1111: grid_color[currentLocation][4:0] <= 5'b111_11;
                    endcase 
                    case(positionData[5:4])
                        2'b00: grid_color[currentLocation][6:5] <= 2'b00;
                        2'b01: grid_color[currentLocation][6:5] <= 2'b01;
                        2'b10: grid_color[currentLocation][6:5] <= 2'b10;
                        2'b11: grid_color[currentLocation][6:5] <= 2'b11;
                    endcase
                end
                else if (positionData[7] == 1'b0) begin // If MSB == 0, then this is robot info
                    currentLocation <= positionData[4:0];
                    currentOrientation <= positionData[6:5];
                end


            end
            
            previousPackage <= currentPackage;
            
            // If transmitting data
            if (packageSending == 1'b1) begin
                // GPIO 31 pin is SPI Clock, GPIO 29 pin is SPI Data
                currSPIClock <= GPIO_0_D[31];
                SPIData <= GPIO_0_D[29];

                if (currSPIClock == 1'b1 && prevSPIClock == 1'b0) begin  //Rising edge of Clock
                    positionData[currDataBits] <= SPIData;
                    currDataBits <= currDataBits - 3'd1;    //Move to the next bit
                end

                prevSPIClock <= currSPIClock;
            end
            
     end
     
    //---DEBUG----Display 8 bit data in LED

    assign LED[0] = currentLocation[0];
    assign LED[1] = currentLocation[1];
    assign LED[2] = currentLocation[2];
    assign LED[3] = currentLocation[3];
    assign LED[4] = currentLocation[4];
    assign LED[5] = currentLocation[5];


    reg [4:0] block;
    

    //Machine to draw to screen
        // Grid Color logic
            // bit 7: Unused
            // bits 6-5: treasure info
            // bit 4: Node has been visited
            // bits 3-0: Wall info


    always @ (*) begin 
        if (PX_X > 0*block_width && PX_X < 1*block_width && PX_Y > 0*block_width && PX_Y < 1*block_width) 
        begin
            // Has it not been visited? 
            if (grid_color[0][4] == 1'b0) PX_color = red;
            // Is the robot currently there?
            else if ((currentLocation == 5'd0) && (PX_X > 0*block_width + 35) && (PX_X < 1*block_width - 35) && (PX_Y > 0*block_width + 35) && (PX_Y < 1*block_width - 35)) PX_color = green;
            // What are the walls around this node?
            else if ((grid_color[0][0] == 1'b1) && (PX_Y < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[0][1] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall;
            else if ((grid_color[0][2] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[0][3] == 1'b1) && (PX_X > (1*block_width - 10))) PX_color = wall;
            // What treasure is at this node?
            else if (grid_color[0][6:5] == 2'b01) PX_color = Tr07;    //  7kHz treasure turns screen blue  
            else if (grid_color[0][6:5] == 2'b10) PX_color = Tr12;    // 12kHz treasure turns screen orange
            else if (grid_color[0][6:5] == 2'b11) PX_color = Tr17;    // 17kHz treasure turns screen purple
            // Otherwise leave it white!
            else PX_color = white;
        end
        
        else if (PX_X > 1*block_width && PX_X < 2*block_width && PX_Y > 0*block_width && PX_Y < 1*block_width) 
        begin
            if (grid_color[1][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd1) && (PX_X > 1*block_width + 35) && (PX_X < 2*block_width - 35) && (PX_Y > 0*block_width + 35) && (PX_Y < 1*block_width - 35)) PX_color = green;
            else if ((grid_color[1][0] == 1'b1) && (PX_Y < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[1][1] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall;
            else if ((grid_color[1][2] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[1][3] == 1'b1) && (PX_X > (2*block_width - 10))) PX_color = wall;
            else if (grid_color[1][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[1][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[1][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 2*block_width && PX_X < 3*block_width && PX_Y > 0*block_width && PX_Y < 1*block_width) 
        begin
            if (grid_color[2][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd2) && (PX_X > 2*block_width + 35) && (PX_X < 3*block_width - 35) && (PX_Y > 0*block_width + 35) && (PX_Y < 1*block_width - 35)) PX_color = green;
            else if ((grid_color[2][0] == 1'b1) && (PX_Y < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[2][1] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall;
            else if ((grid_color[2][2] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[2][3] == 1'b1) && (PX_X > (3*block_width - 10))) PX_color = wall;
            else if (grid_color[2][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[2][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[2][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 3*block_width && PX_X < 4*block_width && PX_Y > 0*block_width && PX_Y < 1*block_width) 
        begin
            if (grid_color[3][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd3) && (PX_X > 3*block_width + 35) && (PX_X < 4*block_width - 35) && (PX_Y > 0*block_width + 35) && (PX_Y < 1*block_width - 35)) PX_color = green;
            else if ((grid_color[3][0] == 1'b1) && (PX_Y < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[3][1] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall;
            else if ((grid_color[3][2] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[3][3] == 1'b1) && (PX_X > (4*block_width - 10))) PX_color = wall;
            else if (grid_color[3][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[3][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[3][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 0*block_width && PX_X < 1*block_width && PX_Y > 1*block_width && PX_Y < 2*block_width) 
        begin
            if (grid_color[4][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd4) && (PX_X > 0*block_width + 35) && (PX_X < 1*block_width - 35) && (PX_Y > 1*block_width + 35) && (PX_Y < 2*block_width - 35)) PX_color = green;
            else if ((grid_color[4][0] == 1'b1) && (PX_Y < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[4][1] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[4][2] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[4][3] == 1'b1) && (PX_X > (1*block_width - 10))) PX_color = wall;
            else if (grid_color[4][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[4][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[4][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 1*block_width && PX_X < 2*block_width && PX_Y > 1*block_width && PX_Y < 2*block_width) 
        begin
            if (grid_color[5][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd5) && (PX_X > 1*block_width + 35) && (PX_X < 2*block_width - 35) && (PX_Y > 1*block_width + 35) && (PX_Y < 2*block_width - 35)) PX_color = green;
            else if ((grid_color[5][0] == 1'b1) && (PX_Y < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[5][1] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[5][2] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[5][3] == 1'b1) && (PX_X > (2*block_width - 10))) PX_color = wall;
            else if (grid_color[5][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[5][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[5][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 2*block_width && PX_X < 3*block_width && PX_Y > 1*block_width && PX_Y < 2*block_width) 
        begin
            if (grid_color[6][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd6) && (PX_X > 2*block_width + 35) && (PX_X < 3*block_width - 35) && (PX_Y > 1*block_width + 35) && (PX_Y < 2*block_width - 35)) PX_color = green;
            else if ((grid_color[6][0] == 1'b1) && (PX_Y < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[6][1] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[6][2] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[6][3] == 1'b1) && (PX_X > (3*block_width - 10))) PX_color = wall;
            else if (grid_color[6][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[6][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[6][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 3*block_width && PX_X < 4*block_width && PX_Y > 1*block_width && PX_Y < 2*block_width) 
        begin
            if (grid_color[7][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd7) && (PX_X > 3*block_width + 35) && (PX_X < 4*block_width - 35) && (PX_Y > 1*block_width + 35) && (PX_Y < 2*block_width - 35)) PX_color = green;
            else if ((grid_color[7][0] == 1'b1) && (PX_Y < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[7][1] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[7][2] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[7][3] == 1'b1) && (PX_X > (4*block_width - 10))) PX_color = wall;
            else if (grid_color[7][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[7][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[7][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 0*block_width && PX_X < 1*block_width && PX_Y > 2*block_width && PX_Y < 3*block_width) 
        begin
            if (grid_color[8][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd8) && (PX_X > 0*block_width + 35) && (PX_X < 1*block_width - 35) && (PX_Y > 2*block_width + 35) && (PX_Y < 3*block_width - 35)) PX_color = green;
            else if ((grid_color[8][0] == 1'b1) && (PX_Y < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[8][1] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[8][2] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[8][3] == 1'b1) && (PX_X > (1*block_width - 10))) PX_color = wall;
            else if (grid_color[8][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[8][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[8][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 1*block_width && PX_X < 2*block_width && PX_Y > 2*block_width && PX_Y < 3*block_width) 
        begin
            if (grid_color[9][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd9) && (PX_X > 1*block_width + 35) && (PX_X < 2*block_width - 35) && (PX_Y > 2*block_width + 35) && (PX_Y < 3*block_width - 35)) PX_color = green;
            else if ((grid_color[9][0] == 1'b1) && (PX_Y < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[9][1] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[9][2] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[9][3] == 1'b1) && (PX_X > (2*block_width - 10))) PX_color = wall;
            else if (grid_color[9][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[9][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[9][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 2*block_width && PX_X < 3*block_width && PX_Y > 2*block_width && PX_Y < 3*block_width) 
        begin
            if (grid_color[10][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd10) && (PX_X > 2*block_width + 35) && (PX_X < 3*block_width - 35) && (PX_Y > 2*block_width + 35) && (PX_Y < 3*block_width - 35)) PX_color = green;
            else if ((grid_color[10][0] == 1'b1) && (PX_Y < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[10][1] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[10][2] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[10][3] == 1'b1) && (PX_X > (3*block_width - 10))) PX_color = wall;
            else if (grid_color[10][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[10][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[10][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 3*block_width && PX_X < 4*block_width && PX_Y > 2*block_width && PX_Y < 3*block_width) 
        begin
            if (grid_color[11][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd11) && (PX_X > 3*block_width + 35) && (PX_X < 4*block_width - 35) && (PX_Y > 2*block_width + 35) && (PX_Y < 3*block_width - 35)) PX_color = green;
            else if ((grid_color[11][0] == 1'b1) && (PX_Y < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[11][1] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[11][2] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[11][3] == 1'b1) && (PX_X > (4*block_width - 10))) PX_color = wall;
            else if (grid_color[11][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[11][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[11][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 0*block_width && PX_X < 1*block_width && PX_Y > 3*block_width && PX_Y < 4*block_width) 
        begin
            if (grid_color[12][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd12) && (PX_X > 0*block_width + 35) && (PX_X < 1*block_width - 35) && (PX_Y > 3*block_width + 35) && (PX_Y < 4*block_width - 35)) PX_color = green;
            else if ((grid_color[12][0] == 1'b1) && (PX_Y < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[12][1] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[12][2] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[12][3] == 1'b1) && (PX_X > (1*block_width - 10))) PX_color = wall;
            else if (grid_color[12][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[12][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[12][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 1*block_width && PX_X < 2*block_width && PX_Y > 3*block_width && PX_Y < 4*block_width) 
        begin
            if (grid_color[13][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd13) && (PX_X > 1*block_width + 35) && (PX_X < 2*block_width - 35) && (PX_Y > 3*block_width + 35) && (PX_Y < 4*block_width - 35)) PX_color = green;
            else if ((grid_color[13][0] == 1'b1) && (PX_Y < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[13][1] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[13][2] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[13][3] == 1'b1) && (PX_X > (2*block_width - 10))) PX_color = wall;
            else if (grid_color[13][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[13][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[13][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 2*block_width && PX_X < 3*block_width && PX_Y > 3*block_width && PX_Y < 4*block_width) 
        begin
            if (grid_color[14][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd14) && (PX_X > 2*block_width + 35) && (PX_X < 3*block_width - 35) && (PX_Y > 3*block_width + 35) && (PX_Y < 4*block_width - 35)) PX_color = green;
            else if ((grid_color[14][0] == 1'b1) && (PX_Y < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[14][1] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[14][2] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[14][3] == 1'b1) && (PX_X > (3*block_width - 10))) PX_color = wall;
            else if (grid_color[14][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[14][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[14][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 3*block_width && PX_X < 4*block_width && PX_Y > 3*block_width && PX_Y < 4*block_width) 
        begin
            if (grid_color[15][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd15) && (PX_X > 3*block_width + 35) && (PX_X < 4*block_width - 35) && (PX_Y > 3*block_width + 35) && (PX_Y < 4*block_width - 35)) PX_color = green;
            else if ((grid_color[15][0] == 1'b1) && (PX_Y < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[15][1] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[15][2] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[15][3] == 1'b1) && (PX_X > (4*block_width - 10))) PX_color = wall;
            else if (grid_color[15][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[15][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[15][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 0*block_width && PX_X < 1*block_width && PX_Y > 4*block_width && PX_Y < 5*block_width) 
        begin
            if (grid_color[16][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd16) && (PX_X > 0*block_width + 35) && (PX_X < 1*block_width - 35) && (PX_Y > 4*block_width + 35) && (PX_Y < 5*block_width - 35)) PX_color = green;
            else if ((grid_color[16][0] == 1'b1) && (PX_Y < (4*block_width + 10))) PX_color = wall;
            else if ((grid_color[16][1] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[16][2] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
            else if ((grid_color[16][3] == 1'b1) && (PX_X > (1*block_width - 10))) PX_color = wall;
            else if (grid_color[16][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[16][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[16][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 1*block_width && PX_X < 2*block_width && PX_Y > 4*block_width && PX_Y < 5*block_width) 
        begin
            if (grid_color[17][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd17) && (PX_X > 1*block_width + 35) && (PX_X < 2*block_width - 35) && (PX_Y > 4*block_width + 35) && (PX_Y < 5*block_width - 35)) PX_color = green;
            else if ((grid_color[17][0] == 1'b1) && (PX_Y < (4*block_width + 10))) PX_color = wall;
            else if ((grid_color[17][1] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[17][2] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
            else if ((grid_color[17][3] == 1'b1) && (PX_X > (2*block_width - 10))) PX_color = wall;
            else if (grid_color[17][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[17][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[17][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else if (PX_X > 2*block_width && PX_X < 3*block_width && PX_Y > 4*block_width && PX_Y < 5*block_width) 
        begin
            if (grid_color[18][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd18) && (PX_X > 2*block_width + 35) && (PX_X < 3*block_width - 35) && (PX_Y > 4*block_width + 35) && (PX_Y < 5*block_width - 35)) PX_color = green;
            else if ((grid_color[18][0] == 1'b1) && (PX_Y < (4*block_width + 10))) PX_color = wall;
            else if ((grid_color[18][1] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[18][2] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
            else if ((grid_color[18][3] == 1'b1) && (PX_X > (3*block_width - 10))) PX_color = wall;
            else if (grid_color[18][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[18][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[18][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end

        else if (PX_X > 3*block_width && PX_X < 4*block_width && PX_Y > 4*block_width && PX_Y < 5*block_width) 
        begin
            if (grid_color[19][4] == 1'b0) PX_color = red;
            else if ((currentLocation == 5'd19) && (PX_X > 3*block_width + 35) && (PX_X < 4*block_width - 35) && (PX_Y > 4*block_width + 35) && (PX_Y < 5*block_width - 35)) PX_color = green;
            else if ((grid_color[19][0] == 1'b1) && (PX_Y < (4*block_width + 10))) PX_color = wall;
            else if ((grid_color[19][1] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[19][2] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
            else if ((grid_color[19][3] == 1'b1) && (PX_X > (4*block_width - 10))) PX_color = wall;
            else if (grid_color[19][6:5] == 2'b01) PX_color = Tr07;
            else if (grid_color[19][6:5] == 2'b10) PX_color = Tr12;
            else if (grid_color[19][6:5] == 2'b11) PX_color = Tr17;
            else PX_color = white;
        end
        
        else begin  
            PX_color = black;
        end
    end 

endmodule
