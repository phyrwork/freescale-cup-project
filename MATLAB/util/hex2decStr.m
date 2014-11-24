function [ decInput ] = hex2decStr( hexInput )
% Converts a 32 character hex string to a 16-byte input or key for AES

    ind = find(hexInput == ' ');
    if (~isempty(ind)) 
        hexInput(ind) = [];
    end

    byte = uint8([]);
    for i = 1 : (length(hexInput) / 2)
        hexByte(i,:) = hexInput( (2*i)-1 : 2*i );
        byte(i) = hex2dec(hexByte(i,:));
    end
    
    decInput = byte;

end

