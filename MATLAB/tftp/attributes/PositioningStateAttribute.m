classdef PositioningStateAttribute < TftpAttribute
    %PositioningStateAttribute 
 
    methods
        % PositioningStateAttribute constructor
        function obj = PositioningStateAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 1;
            ssize = 1;
            ctype = 'uint8';
            mtype = 'PositioningStateType';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function value = decode(obj, value)
            % cast byte stream to
            %value = typecast(value, obj.ctype);
            
            % fetch correct enumeration
            value = PositioningStateType(value);
        end
        
        % encode
        function value = encode(obj, value)

            % cast to ctype
            value = cast(value, obj.ctype);  
        end
    end   
end