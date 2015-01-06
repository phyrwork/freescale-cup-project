classdef PidAttribute < TftpAttribute
    %PidAttribute
 
    methods
        % PidAttribute constructor
        function obj = PidAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 36;
            ssize = 1;
            ctype = 'uint8';
            mtype = 'PidType';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function pid = decode(obj, stream)
            % cast byte stream
            %stream = typecast(stream, 'uint8');
            
            % initialise object
            pid = PidType(stream);
        end
        
        % encode
        function stream = encode(obj, pid)
            % build stream from PidType properties
            stream = pid.pack();
            
            % cast to byte stream
            %stream = cast(stream, 'uint8');
        end
    end   
end