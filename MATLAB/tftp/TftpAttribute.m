classdef TftpAttribute < TftpModule
    %TftpAttribute
    
    properties
        attribute = cast([], 'char');   % attribute name
        ssize     = cast([], 'uint16'); % sample width
        ctype     = cast([], 'char');   % native data type
        mtype     = cast([], 'char');   % convenient MATLAB data type
    end
    
    methods
        % TftpAttribute constructor
        function obj = TftpAttribute(code, attribute, fsize, ssize, ctype, mtype)
            % parse input
            p = inputParser();
            addRequired(p, 'code', @ischar);
            addRequired(p, 'attribute', @ischar);
            addRequired(p, 'fsize', @isnumeric);
            addRequired(p, 'ssize', @isnumeric);
            addRequired(p, 'ctype', @ischar);
            addRequired(p, 'mtype', @ischar);
            parse(p, code, attribute, fsize, ssize, ctype, mtype);
            
            % set properties
            obj.ptype     = cast('attribute', 'char');
            obj.code      = cast(hex2dec(p.Results.code), 'uint8');
            obj.attribute = cast(p.Results.attribute, 'char');
            obj.fsize     = cast(p.Results.fsize, 'uint16');
            obj.ssize     = cast(p.Results.ssize, 'uint16');
            obj.ctype     = cast(p.Results.ctype, 'char');
            obj.mtype     = cast(p.Results.mtype, 'char');
        end
        
        % convert from bytes to ctype
        function value = decode(obj, value)
            % convert uint8(256) to ctype[]
            value = flip(value); % big to little endian
            value = typecast(value, obj.ctype); % cast bytes back to ctype
            value = flip(value); % little to big endian
            
            % cast to mtype[]
            if ( ~strcmp(obj.ctype, obj.mtype) )
                value = cast(value, obj.mtype);
            end
        end
        
        % convert from ctype to bytes
        function value = encode(obj, value)
            % cast to ctype[]
            value = cast(value, obj.ctype);
            
            % convert ctype[] to uint8(256)
            value = flip(value);
            value = typecast(value, 'uint8');
            value = flip(value);
        end       
    end
end

