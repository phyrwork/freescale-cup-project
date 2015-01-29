function [ output ] = cobs_decode( input, mtu )
%cobs_decode Decode string with COBS

    % Unstuffs 'input' into 'output'
    output = zeros(0, mtu, 'uint8');
    r = 1;

    while( r < length(input) )
        
        code = input(r);                            % number of bytes to copy
        output(end+1:end+code-1) = input(r+1:r+code-1); % copy bytes

        if( code ~= 255 && r ~= length(input) ) % 255 = 0xFF
            output(end+1) = 0; % 0 = '\0'
        end
        
        r = r + code;
    end 
end

