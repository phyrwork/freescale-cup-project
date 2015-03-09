classdef LineAttribute < TftpAttribute
    %LineAttribute
 
    methods
        % LineAttribute constructor
        function obj = LineAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 20;
            ssize = 1;
            ctype = 'uint8';
            mtype = 'LineType';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function line = decode(obj, stream)
            % cast byte stream
            %stream = typecast(stream, 'uint8');
            
            % initialise object
            line = LineType(flip(stream));
        end
        
        % encode
        function stream = encode(obj, line)
            % build stream from EdgeType properties
            stream = flip(line.pack());
            
            % cast to byte stream
            %stream = cast(stream, 'uint8');
        end
    end   
end