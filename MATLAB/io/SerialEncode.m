function [ output ] = SerialEncode( input )
%SerialEncode
    
    % Perform COBS and double check output is uint8[]
    output = cobs_encode(input);
    output(length(output) + 1) = 0;
end

