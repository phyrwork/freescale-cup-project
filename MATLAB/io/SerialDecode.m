function [ valid, output ] = SerialDecode( input )
%SerialDecode
    
    % Ensure input is uint8[], un-COBS
    input = input(1:length(input) - 1);
    input = uint8(cobs_decode(input));
    
    % Get checksums
    crc_i = input(length(input) - 1:length(input));
    crc_i = flip(crc_i);
    crc_i = typecast(crc_i, 'uint16');
    crc_a = uint16(crc16(hex2dec('FFFF'), input(1:length(input) - 2)));
    
    % Verify checksums
    if (crc_i ~= crc_a)
        output = char('Checksum miss. Discarding message.');
        valid = false;
        return;
    end
    
    % Return output
    output = input(3:length(input) - 2);
    output = uint8(output);
    valid = true;
end