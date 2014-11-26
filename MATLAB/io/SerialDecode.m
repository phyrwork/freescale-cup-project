function [ output ] = SerialDecode( input )
%SerialDecode
    
    % Ensure input is uint8[], un-COBS
    output = uint8([]);
    try
        output = uint8(cobs_decode(input(1:length(input) - 1)));
    catch
        error('Error unstuffing frame.');
    end
end