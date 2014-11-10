function [ output ] = SerialEncode( input )
%SerialEncode
    
    % Ensure input is uint8[], set up write
    input = uint8(input);
    write_index = 1;
    
    
    % Add length to output
    len = uint16(length(input)); % Get value
    len = typecast(len, 'uint8'); % Cast to uint8[]...
    len = flip(len); % ...convert to big endian.
    
    output(write_index:write_index + length(len) - 1) = len;
    write_index = write_index + length(len);
    
    
    % Add input to output
    output(write_index:write_index + length(input) - 1) = input;
    write_index = write_index + length(input);
    
    
    % Calculate and add checksum to output
    crc = uint16(crc16(hex2dec('FFFF'), output));
    crc = typecast(crc, 'uint8');
    crc = flip(crc);
    
    output(write_index:write_index + length(crc) - 1) = crc;
    write_index = write_index + length(crc);
    
    
    % Perform COBS and double check output is uint8[]
    output = cobs_encode(output);
    output(length(output) + 1) = 0;
    output = uint8(output);
end

