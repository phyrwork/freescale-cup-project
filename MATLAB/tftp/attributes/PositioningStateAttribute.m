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
            value = typecast(value, obj.ctype);
            
            % fetch correct enumeration
            switch (value)
                case 0
                    value = PositioningStateType.full;
                case 10
                    value = PositioningStateType.partial_left;
                case 20
                    value = PositioningStateType.partial_right;
                case 255
                    value = PositioningStateType.none;
                otherwise
            end
        end
        
        % encode
        function value = encode(obj, value)
            % fetch correct integer value
            switch (value)
                case PositioningStateType.full
                    value = 0;
                case PositioningStateType.partial_left
                    value = 10;
                case PositioningStateType.partial_right
                    value = 20;
                case PositioningStateType.none
                    value = 255;
                otherwise
            end
            
            % cast to ctype
            value = cast(value, obj.ctype);  
        end
    end   
end