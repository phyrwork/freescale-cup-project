function [ crc ] = crc16( crc_in, input )
%crc 16-bit cyclic redundancy check
%   
    % Ported from the C version used on the FRDM-KL25Z
    poly = uint16(hex2dec('8408'));
    crc = uint32(crc_in);
    len = length(input);
    
    if (isempty(input))
        crc = bitcmp(crc); % This shouldn't occur.
    else
        % do (while)
        while (true)
            
            % for (i=0, data=(uint32_t)0xff & *data_p++;
            data = bitand(uint32(hex2dec('FF')), uint32(uint8(input(length(input)-len+1))));
            %      i < 8; i++
            for i = 0:1:7
                % if ((crc & 0x0001) ^ (data & 0x0001))
                if (bitxor(bitand(crc, uint32(hex2dec('0001'))), bitand(data, uint32(hex2dec('0001')))));
                    % crc = (crc >> 1) ^ POLY;
                    crc = bitxor(bitshift(crc, -1), uint32(poly));
                else
                    % else  crc >>= 1;
                    crc = bitshift(crc, -1);
                end  
            % data >>= 1) //end for()
                data = bitshift(data, -1);
            end
            
            % while (--length);
            len = len - 1;
            if (len == 0)
                break;
            end
        end
        
        crc = bitcmp(crc);
        data = crc;
        crc = bitor(bitshift(crc, 8), bitand(bitshift(data, -8), uint32(hex2dec('FF'))));
        crc = typecast(crc, 'uint16');
        crc = crc(1);
    end  
end

