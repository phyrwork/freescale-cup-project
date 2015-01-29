classdef WheelSpeedAttribute < TftpAttribute
    %WheelSpeedAttribute 

    methods
        % constructor
        function obj = WheelSpeedAttribute(code, attribute)
            
            % set default properties used in superclass constructor
            ctype = 'single';
            mtype = 'single';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, ctype, mtype);
        end
            
        % decode
        function value = decode(obj, value)
            
            % cast byte stream
            value = decode@TftpAttribute(obj, value);
            
            % convert revolutions per second to per minute
            value = value * 60;
        end
        
        % encode
        function value = encode(obj, value)
            
            % convert revolutions per minute to per second
            value = value / 60;
            
            % cast to byte stream
            value = encode@TftpAttribute(obj, value);
        end
    end  
end