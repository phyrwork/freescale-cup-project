function [ output ] = SerialDecode( input )
%SerialDecode
    
    % Ensure input is uint8[], un-COBS
    input = input(1:length(input) - 1);
    try
        input = uint8(cobs_decode(input));
    catch
        output = uint8([]);
        error('Error unstuffing frame.');
    end
    
    % Get checksums
    crc_i = input(length(input) - 1:length(input));
    crc_i = flip(crc_i);
    crc_i = typecast(crc_i, 'uint16');
    crc_a = uint16(crc16(65535, input(1:length(input) - 2)));
    
    % Verify checksums
    if (crc_i ~= crc_a)
        error('Checksum miss. Discarding message.');
    end
    
    % Return output
    output = input(3:length(input) - 2);
    output = uint8(output);
end