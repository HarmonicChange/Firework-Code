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

     // Local parameters for notes
    localparam CLKDIVIDERC = ONE_SEC/(256*261);
    localparam CLKDIVIDERD = ONE_SEC/(256*293);
    localparam CLKDIVIDERE = ONE_SEC/(256*329);
    localparam CLKDIVIDERF = ONE_SEC/(256*349);
    localparam CLKDIVIDERG = ONE_SEC/(256*392);
    localparam CLKDIVIDERA = ONE_SEC/(256*440);
    localparam CLKDIVIDERCsh = ONE_SEC/(256*554);
    //localparam CLKDIVIDERE = ONE_SEC/(256*660);
         
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
    reg         led_state;     // 1 is on, 0 is off

    // Variables for DONE song
    wire done;
    reg [19:0] visited; // each bit signifies a node has been visited, used for done signal
    wire [7:0]  DAC; //8-bit output to DAC
    reg [7:0] addr;
    reg [6:0] song_pos;
    wire [2:0] note; //note being played     
    reg [15:0] counter; //counts cycles until moving to next ROM address
    reg [25:0] note_length; //counts cycles until moving to next note
     
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

    //Wired up ROM table
    rom Table (
       .clock(CLOCK_50),
       .addr(addr),
       .sine(DAC)
    );
     
    twinkle Song (
       .clock(CLOCK_50),
       .addr(song_pos),
       .note(note)
    );
     
    assign reset = ~KEY[0]; // reset when KEY0 is pressed
    assign GPIO_1_D[7:0] = DAC;
    assign done = (visited == 20'b1111_1111_1111_1111_1111);
         
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
     
    // Store grid info
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
        visited = 20'd0;
    end
     
    //SPI receiving
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
            
            // If MSB == 1, then this is maze info
            if (positionData[7] == 1'b1) begin      
                grid_color[currentLocation][3:0] <= positionData[3:0];  // Wall info
                grid_color[currentLocation][4]   <= 1'b1;               // Node is visited
                grid_color[currentLocation][6:5] <= positionData[5:4];  // Treasure info
            end

            // If MSB == 0, then this is robot info
            else if (positionData[7] == 1'b0) begin 
                currentLocation <= positionData[4:0];
                currentOrientation <= positionData[6:5];
                visited[currentLocation] <= 1'b1;
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

    // FSM for done signal
    always @ (posedge CLOCK_50) begin
        if (done == 1'b1) begin
            //State machine switching between notes every second
            if (note_length == 0) begin
                note_length <= ONE_SEC/4;
                if (song_pos == 7'd87) //song_pos <= 0;
                else                   song_pos <= song_pos + 1;
                //note <= note + 1;
            end
            else note_length <= note_length - 1;
            
            /*State machine to output ROM sin value after
              a specified sin period based on note*/
            if (counter == 0) begin
                case(note)
                    3'b000: counter    <= 0;
                    3'b001: counter    <= CLKDIVIDERC - 1;
                    3'b010: counter    <= CLKDIVIDERD - 1;
                    3'b011: counter    <= CLKDIVIDERE - 1;
                    3'b100: counter    <= CLKDIVIDERF - 1;
                    3'b101: counter    <= CLKDIVIDERG - 1;
                    3'b110: counter    <= CLKDIVIDERA - 1;
                    3'b111: counter    <= CLKDIVIDERCsh - 1;    
                endcase
            
                if (addr < 256) addr <= addr+1;
                else            addr <= 0;
            end
            else counter <= counter - 1;
        end
    end 
     
    //---DEBUG----Display 8 bit data in LED
    assign LED[0] = currentLocation[0];
    assign LED[1] = currentLocation[1];
    assign LED[2] = currentLocation[2];
    assign LED[3] = currentLocation[3];
    assign LED[4] = currentLocation[4];
    assign LED[5] = currentLocation[5];

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
            else if ((grid_color[0][0] == 1'b1) && (PX_Y < (0*block_width + 10))) PX_color = wall; // Top
            else if ((grid_color[0][2] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall; // Bottom
            else if ((grid_color[0][1] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall; // Left
            else if ((grid_color[0][3] == 1'b1) && (PX_X > (1*block_width - 10))) PX_color = wall; // Right
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
            else if ((grid_color[1][2] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall;
            else if ((grid_color[1][1] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[2][2] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall;
            else if ((grid_color[2][1] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[3][2] == 1'b1) && (PX_Y > (1*block_width - 10))) PX_color = wall;
            else if ((grid_color[3][1] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[4][2] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[4][1] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[5][2] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[5][1] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[6][2] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[6][1] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[7][2] == 1'b1) && (PX_Y > (2*block_width - 10))) PX_color = wall;
            else if ((grid_color[7][1] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[8][2] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[8][1] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[9][2] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[9][1] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[10][2] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[10][1] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[11][2] == 1'b1) && (PX_Y > (3*block_width - 10))) PX_color = wall;
            else if ((grid_color[11][1] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[12][2] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[12][1] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[13][2] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[13][1] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[14][2] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[14][1] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[15][2] == 1'b1) && (PX_Y > (4*block_width - 10))) PX_color = wall;
            else if ((grid_color[15][1] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[16][2] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[16][1] == 1'b1) && (PX_X < (0*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[17][2] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[17][1] == 1'b1) && (PX_X < (1*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[18][2] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[18][1] == 1'b1) && (PX_X < (2*block_width + 10))) PX_color = wall;
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
            else if ((grid_color[19][2] == 1'b1) && (PX_Y > (5*block_width - 10))) PX_color = wall;
            else if ((grid_color[19][1] == 1'b1) && (PX_X < (3*block_width + 10))) PX_color = wall;
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

module twinkle(clock,addr,note);
 input clock;
 input [6:0] addr;
 output [2:0] note;
 reg    [2:0] note;
 always@(posedge clock) begin
   case(addr)
       7'd0: note = 3'd1;
        7'd1: note = 3'd0;
        7'd2: note = 3'd1;
        7'd3: note = 3'd0;
        7'd4: note = 3'd5;
        7'd5: note = 3'd0;
        7'd6: note = 3'd5;
        7'd7: note = 3'd0;
        7'd8: note = 3'd6;
        7'd9: note = 3'd0;
        7'd10: note = 3'd6;
        7'd11: note = 3'd0;
        7'd12: note = 3'd5;
        7'd13: note = 3'd0;
        7'd14: note = 3'd4;
        7'd15: note = 3'd0;
        7'd16: note = 3'd4;
        7'd17: note = 3'd0;
        7'd18: note = 3'd3;
        7'd19: note = 3'd0;
        7'd20: note = 3'd3;
        7'd21: note = 3'd0;
        7'd22: note = 3'd2;
        7'd23: note = 3'd0;
        7'd24: note = 3'd2;
        7'd25: note = 3'd0;
        7'd26: note = 3'd1;
        7'd27: note = 3'd0;
        7'd28: note = 3'd5;
        7'd29: note = 3'd0;
        7'd30: note = 3'd5;
        7'd31: note = 3'd0;
        7'd32: note = 3'd4;
        7'd33: note = 3'd0;
        7'd34: note = 3'd4;
        7'd35: note = 3'd0;
        7'd36: note = 3'd3;
        7'd37: note = 3'd0;
        7'd38: note = 3'd3;
        7'd39: note = 3'd0;
        7'd40: note = 3'd2;
        7'd41: note = 3'd0;
        7'd42: note = 3'd0;
        7'd43: note = 3'd5;
        7'd44: note = 3'd0;
        7'd45: note = 3'd5;
        7'd46: note = 3'd0;
        7'd47: note = 3'd4;
        7'd48: note = 3'd0;
        7'd49: note = 3'd4;
        7'd50: note = 3'd0;
        7'd51: note = 3'd3;
        7'd52: note = 3'd0;
        7'd53: note = 3'd3;
        7'd54: note = 3'd0;
        7'd55: note = 3'd2;
        7'd56: note = 3'd0;
        7'd57: note = 3'd1;
        7'd58: note = 3'd0;
        7'd59: note = 3'd1;
        7'd60: note = 3'd0;
        7'd61: note = 3'd5;
        7'd62: note = 3'd0;
        7'd63: note = 3'd5;
        7'd64: note = 3'd0;
        7'd65: note = 3'd6;
        7'd66: note = 3'd0;
        7'd67: note = 3'd6;
        7'd68: note = 3'd0;
        7'd69: note = 3'd5;
        7'd70: note = 3'd0;
        7'd71: note = 3'd4;
        7'd72: note = 3'd0;
        7'd73: note = 3'd4;
        7'd74: note = 3'd0;
        7'd75: note = 3'd3;
        7'd76: note = 3'd0;
        7'd77: note = 3'd3;
        7'd78: note = 3'd0;
        7'd79: note = 3'd2;
        7'd80: note = 3'd0;
        7'd81: note = 3'd2;
        7'd82: note = 3'd0;
        7'd83: note = 3'd1;
        7'd84: note = 3'd0;
        7'd85: note = 3'd0;
        7'd86: note = 3'd0;
        7'd87: note = 3'd0;
        
        default: note = 3'd0;
        
    endcase
 end
endmodule