function [ output ] = cobs_decode( input )
%cobs_decode Decode string with COBS

    % Unstuffs 'input' into 'output'
    output = uint8([]);
    read_index = 0;
    write_index = 0;

    while( read_index < length(input) )
        
        code = input(read_index+1);

        if(read_index + code > length(input) && code ~= 1)
            error('Encoded message is invalid: byte code exceeds remaining undecoded length.');
        end

        read_index = read_index + 1;

        for i = 1:1:code-1
            output(write_index+1) = input(read_index+1);
            write_index = write_index + 1;
            read_index = read_index + 1;
        end
        if( code ~= 255 && read_index ~= length(input) ) % 255 = 0xFF
            output(write_index+1) = 0; % 0 = '\0'
            write_index = write_index + 1;
        end
    end 
end

