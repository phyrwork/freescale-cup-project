classdef TftpAttribute
    %TftpAttribute
    
    properties
        code =      cast([], 'uint8');
        attribute = cast([], 'char');
        fsize =     cast([], 'uint16');
        ctype =     cast([], 'char');
        mtype =     cast([], 'char');
    end
    
    methods
        % TftpAttribute constructor
        function obj = TftpAttribute(code, attribute, fsize, ctype, mtype)
            % prepare validation functions
            isuint8 = @(x) isa(x, 'uint8');
            isuint16 = @(x) isa(x, 'uin16t');
            
            % parse input
            p = inputParser();
            addRequired(p, 'code', isuint8);
            addRequired(p, 'attribute', @ischar);
            addRequired(p, 'fsize', isuint16);
            addRequired(p, 'ctype', @ischar);
            addRequired(p, 'mtype', @ischar);
            
            
            % set properties
            obj.code        = cast(code, 'uint8');
            obj.attribute   = cast(attribute, 'char');
            obj.fsize       = cast(fsize, 'uint16');
            obj.ctype       = cast(ctype, 'char');
            obj.mtype       = cast(mtype, 'char');
        end
        
        % convert from bytes to ctype
        function value = decode(obj, value)
            % convert uint8(256) to uint16(128)
            value = flip(value); % big to little endian
            value = typecast(value, obj.ctype); % cast bytes back to ctype
            value = flip(value); % little to big endian
        end
        
        % convert from ctype to bytes
        function value = encode(obj, value)
            % convert uint16(128) to uint8(256)
            value = flip(value);
            value = typecast(value, 'uint8');
            value = flip(value);
        end       
    end
end

