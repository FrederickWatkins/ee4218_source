module myip (
    input clk,
    input resetn,

    input start, // start will be raised for a single cycle when start is requested
    output done, // done should be raised for a single cycle when accel is done

    output [5:0] input_mem_addr, // 6 bits for 64 rows
    output input_mem_re, // Input memory read enable
    input [7:0][7:0] input_mem_data, // 8 8 bit values

    output [5:0] output_mem_addr, // 6 bits for 64 rows
    output output_mem_we, // Output memory write enable
    output [7:0] output_mem_data, // Single 8 bit value

    input [1:0][7:0][7:0] w_hid, // 8x2 matrix of 8 bit values. Currently column major, you may want to swap
    input [2:0][7:0] w_out // 3x1 matrix
);

    reg [6:0] addr; // 7 bits so it can count up to 64 and idle
    reg [5:0] out_addr;
    reg out_we;

    reg [15:0] result;
    reg [15:0] hidden[1:0];
    reg [7:0] activated[1:0];
    integer i;
    
    always @(posedge clk) begin
        if (!resetn) begin // I assume it's active low
            addr     <= 64; // idle at 64
            out_addr <= 0;
            out_we   <= 0;
        end else if (start) begin
            addr     <= 0;
            out_we   <= 0;
        end else if (addr < 64) begin
            addr <= addr + 1;
            out_addr <= addr[5:0];
            out_we   <= 1;
        end else begin
            out_we   <= 0; // Stop writing once we finish row 63
        end
    end

    assign input_mem_addr  = addr[5:0];
    assign input_mem_re    = (addr < 64);
    
    assign output_mem_addr = out_addr;
    assign output_mem_we   = out_we;

    // combinational math
    always @(*) begin
        hidden[0] = 0;
        hidden[1] = 0;
        
        for (i = 1; i < 8; i = i + 1) begin
            hidden[0] = hidden[0] + {8'b0, input_mem_data[i-1]} * {8'b0, w_hid[0][i]};
            hidden[1] = hidden[1] + {8'b0, input_mem_data[i-1]} * {8'b0, w_hid[1][i]};
        end
        
        hidden[0] = (hidden[0] >> 8) + {8'b0, w_hid[0][0]};
        hidden[1] = (hidden[1] >> 8) + {8'b0, w_hid[1][0]};
        activated[0] = sigmoid(hidden[0]);
        activated[1] = sigmoid(hidden[1]);

        // Output layer
        result = w_out[0] + (({8'b0, activated[0]} * {8'b0, w_out[1]} + {8'b0, activated[1]} * {8'b0, w_out[2]}) >> 8);
    end

    assign output_mem_data = result[7:0];
    
    // pulses high for 1 cycle when the final row is written
    assign done = (out_addr == 63 && out_we == 1);

    function [7:0] sigmoid(input [7:0] x);
    begin
        case(x)
        8'd1: sigmoid = 12;
        8'd2: sigmoid = 12;
        8'd3: sigmoid = 12;
        8'd4: sigmoid = 13;
        8'd5: sigmoid = 13;
        8'd6: sigmoid = 13;
        8'd7: sigmoid = 14;
        8'd8: sigmoid = 14;
        8'd9: sigmoid = 14;
        8'd10: sigmoid = 15;
        8'd11: sigmoid = 15;
        8'd12: sigmoid = 15;
        8'd13: sigmoid = 16;
        8'd14: sigmoid = 16;
        8'd15: sigmoid = 16;
        8'd16: sigmoid = 17;
        8'd17: sigmoid = 17;
        8'd18: sigmoid = 18;
        8'd19: sigmoid = 18;
        8'd20: sigmoid = 18;
        8'd21: sigmoid = 19;
        8'd22: sigmoid = 19;
        8'd23: sigmoid = 20;
        8'd24: sigmoid = 20;
        8'd25: sigmoid = 21;
        8'd26: sigmoid = 21;
        8'd27: sigmoid = 21;
        8'd28: sigmoid = 22;
        8'd29: sigmoid = 22;
        8'd30: sigmoid = 23;
        8'd31: sigmoid = 23;
        8'd32: sigmoid = 24;
        8'd33: sigmoid = 24;
        8'd34: sigmoid = 25;
        8'd35: sigmoid = 26;
        8'd36: sigmoid = 26;
        8'd37: sigmoid = 27;
        8'd38: sigmoid = 27;
        8'd39: sigmoid = 28;
        8'd40: sigmoid = 28;
        8'd41: sigmoid = 29;
        8'd42: sigmoid = 30;
        8'd43: sigmoid = 30;
        8'd44: sigmoid = 31;
        8'd45: sigmoid = 32;
        8'd46: sigmoid = 32;
        8'd47: sigmoid = 33;
        8'd48: sigmoid = 34;
        8'd49: sigmoid = 34;
        8'd50: sigmoid = 35;
        8'd51: sigmoid = 36;
        8'd52: sigmoid = 36;
        8'd53: sigmoid = 37;
        8'd54: sigmoid = 38;
        8'd55: sigmoid = 39;
        8'd56: sigmoid = 39;
        8'd57: sigmoid = 40;
        8'd58: sigmoid = 41;
        8'd59: sigmoid = 42;
        8'd60: sigmoid = 43;
        8'd61: sigmoid = 44;
        8'd62: sigmoid = 44;
        8'd63: sigmoid = 45;
        8'd64: sigmoid = 46;
        8'd65: sigmoid = 47;
        8'd66: sigmoid = 48;
        8'd67: sigmoid = 49;
        8'd68: sigmoid = 50;
        8'd69: sigmoid = 51;
        8'd70: sigmoid = 52;
        8'd71: sigmoid = 53;
        8'd72: sigmoid = 54;
        8'd73: sigmoid = 55;
        8'd74: sigmoid = 56;
        8'd75: sigmoid = 57;
        8'd76: sigmoid = 58;
        8'd77: sigmoid = 59;
        8'd78: sigmoid = 60;
        8'd79: sigmoid = 61;
        8'd80: sigmoid = 62;
        8'd81: sigmoid = 63;
        8'd82: sigmoid = 64;
        8'd83: sigmoid = 66;
        8'd84: sigmoid = 67;
        8'd85: sigmoid = 68;
        8'd86: sigmoid = 69;
        8'd87: sigmoid = 70;
        8'd88: sigmoid = 72;
        8'd89: sigmoid = 73;
        8'd90: sigmoid = 74;
        8'd91: sigmoid = 75;
        8'd92: sigmoid = 76;
        8'd93: sigmoid = 78;
        8'd94: sigmoid = 79;
        8'd95: sigmoid = 80;
        8'd96: sigmoid = 82;
        8'd97: sigmoid = 83;
        8'd98: sigmoid = 84;
        8'd99: sigmoid = 86;
        8'd100: sigmoid = 87;
        8'd101: sigmoid = 88;
        8'd102: sigmoid = 90;
        8'd103: sigmoid = 91;
        8'd104: sigmoid = 92;
        8'd105: sigmoid = 94;
        8'd106: sigmoid = 95;
        8'd107: sigmoid = 97;
        8'd108: sigmoid = 98;
        8'd109: sigmoid = 99;
        8'd110: sigmoid = 101;
        8'd111: sigmoid = 102;
        8'd112: sigmoid = 104;
        8'd113: sigmoid = 105;
        8'd114: sigmoid = 107;
        8'd115: sigmoid = 108;
        8'd116: sigmoid = 110;
        8'd117: sigmoid = 111;
        8'd118: sigmoid = 113;
        8'd119: sigmoid = 114;
        8'd120: sigmoid = 116;
        8'd121: sigmoid = 117;
        8'd122: sigmoid = 119;
        8'd123: sigmoid = 120;
        8'd124: sigmoid = 122;
        8'd125: sigmoid = 123;
        8'd126: sigmoid = 125;
        8'd127: sigmoid = 126;
        8'd128: sigmoid = 128;
        8'd129: sigmoid = 129;
        8'd130: sigmoid = 130;
        8'd131: sigmoid = 132;
        8'd132: sigmoid = 133;
        8'd133: sigmoid = 135;
        8'd134: sigmoid = 136;
        8'd135: sigmoid = 138;
        8'd136: sigmoid = 139;
        8'd137: sigmoid = 141;
        8'd138: sigmoid = 142;
        8'd139: sigmoid = 144;
        8'd140: sigmoid = 145;
        8'd141: sigmoid = 147;
        8'd142: sigmoid = 148;
        8'd143: sigmoid = 150;
        8'd144: sigmoid = 151;
        8'd145: sigmoid = 153;
        8'd146: sigmoid = 154;
        8'd147: sigmoid = 156;
        8'd148: sigmoid = 157;
        8'd149: sigmoid = 158;
        8'd150: sigmoid = 160;
        8'd151: sigmoid = 161;
        8'd152: sigmoid = 163;
        8'd153: sigmoid = 164;
        8'd154: sigmoid = 165;
        8'd155: sigmoid = 167;
        8'd156: sigmoid = 168;
        8'd157: sigmoid = 169;
        8'd158: sigmoid = 171;
        8'd159: sigmoid = 172;
        8'd160: sigmoid = 173;
        8'd161: sigmoid = 175;
        8'd162: sigmoid = 176;
        8'd163: sigmoid = 177;
        8'd164: sigmoid = 179;
        8'd165: sigmoid = 180;
        8'd166: sigmoid = 181;
        8'd167: sigmoid = 182;
        8'd168: sigmoid = 183;
        8'd169: sigmoid = 185;
        8'd170: sigmoid = 186;
        8'd171: sigmoid = 187;
        8'd172: sigmoid = 188;
        8'd173: sigmoid = 189;
        8'd174: sigmoid = 191;
        8'd175: sigmoid = 192;
        8'd176: sigmoid = 193;
        8'd177: sigmoid = 194;
        8'd178: sigmoid = 195;
        8'd179: sigmoid = 196;
        8'd180: sigmoid = 197;
        8'd181: sigmoid = 198;
        8'd182: sigmoid = 199;
        8'd183: sigmoid = 200;
        8'd184: sigmoid = 201;
        8'd185: sigmoid = 202;
        8'd186: sigmoid = 203;
        8'd187: sigmoid = 204;
        8'd188: sigmoid = 205;
        8'd189: sigmoid = 206;
        8'd190: sigmoid = 207;
        8'd191: sigmoid = 208;
        8'd192: sigmoid = 209;
        8'd193: sigmoid = 210;
        8'd194: sigmoid = 211;
        8'd195: sigmoid = 211;
        8'd196: sigmoid = 212;
        8'd197: sigmoid = 213;
        8'd198: sigmoid = 214;
        8'd199: sigmoid = 215;
        8'd200: sigmoid = 216;
        8'd201: sigmoid = 216;
        8'd202: sigmoid = 217;
        8'd203: sigmoid = 218;
        8'd204: sigmoid = 219;
        8'd205: sigmoid = 219;
        8'd206: sigmoid = 220;
        8'd207: sigmoid = 221;
        8'd208: sigmoid = 221;
        8'd209: sigmoid = 222;
        8'd210: sigmoid = 223;
        8'd211: sigmoid = 223;
        8'd212: sigmoid = 224;
        8'd213: sigmoid = 225;
        8'd214: sigmoid = 225;
        8'd215: sigmoid = 226;
        8'd216: sigmoid = 227;
        8'd217: sigmoid = 227;
        8'd218: sigmoid = 228;
        8'd219: sigmoid = 228;
        8'd220: sigmoid = 229;
        8'd221: sigmoid = 229;
        8'd222: sigmoid = 230;
        8'd223: sigmoid = 231;
        8'd224: sigmoid = 231;
        8'd225: sigmoid = 232;
        8'd226: sigmoid = 232;
        8'd227: sigmoid = 233;
        8'd228: sigmoid = 233;
        8'd229: sigmoid = 234;
        8'd230: sigmoid = 234;
        8'd231: sigmoid = 234;
        8'd232: sigmoid = 235;
        8'd233: sigmoid = 235;
        8'd234: sigmoid = 236;
        8'd235: sigmoid = 236;
        8'd236: sigmoid = 237;
        8'd237: sigmoid = 237;
        8'd238: sigmoid = 237;
        8'd239: sigmoid = 238;
        8'd240: sigmoid = 238;
        8'd241: sigmoid = 239;
        8'd242: sigmoid = 239;
        8'd243: sigmoid = 239;
        8'd244: sigmoid = 240;
        8'd245: sigmoid = 240;
        8'd246: sigmoid = 240;
        8'd247: sigmoid = 241;
        8'd248: sigmoid = 241;
        8'd249: sigmoid = 241;
        8'd250: sigmoid = 242;
        8'd251: sigmoid = 242;
        8'd252: sigmoid = 242;
        8'd253: sigmoid = 243;
        8'd254: sigmoid = 243;
        8'd255: sigmoid = 243;
        endcase
    end
    endfunction
endmodule
