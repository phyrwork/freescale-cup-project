classdef MotorCurrentAttribute < TftpAttribute
    %AngularVelocityAttribute
 
    methods
        % MotorCurrentAttribute constructor
        function obj = MotorCurrentAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 2;
            ssize = 1;
            ctype = 'uint16';
            mtype = 'single';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function value = decode(obj, value)
            % get value from ctype to mtype
            value = decode@TftpAttribute(obj, value);
            
            % convert from mtpye to Amps
            value = (value/4095) * 3.3 * 1.2615;
        end
        
        % encode
        function value = encode(obj, value)
            % convert from Amps to mtype
            value = (value/3.3/2.62) * 4095;
            
            % convert from mtype to ctype
            value = encode@TftpAttribute(obj, value);
        end
    end  
end