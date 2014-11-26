function [ output ] = cobs_encode( input )
%cobs_encode Encode string with COBS
    
    % Stuffs bytes of data 'input' to 'output'
    read_index = 0;
    write_index = 1;
    code_index = 0;
    code = 1;

    while( read_index < length(input) )
    
        if(input(read_index+1) == 0)
        
            output(code_index+1) = code;
            code = 1;
            code_index = write_index;
            
            write_index = write_index + 1;
            read_index = read_index + 1;
            
        else
            output(write_index+1) = input(read_index+1);
            write_index = write_index + 1;
            read_index = read_index + 1;
            
            code = code + 1;
            if(code == 255) %255 = 0xFF
            
                output(code_index+1) = code;
                code = 1;
                code_index = write_index;
                write_index = write_index + 1;
            end
        end
    end

    output(code_index+1) = code;
    output = uint8(output);
end

