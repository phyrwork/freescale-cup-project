classdef LineAttribute < TftpAttribute
    %LineAttribute
 
    methods
        % LineAttribute constructor
        function obj = LineAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 49; % 2*18 + 1 + 4 + 4 + 4
            ssize = 1;
            ctype = 'uint8';
            mtype = 'LineType';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function line = decode(stream)
            % cast byte stream
            %stream = typecast(stream, 'uint8');
            
            % initialise object
            line = LineType(stream);
        end
        
        % encode
        function stream = encode(line)
            % build stream from EdgeType properties
            stream = line.pack();
            
            % cast to byte stream
            %stream = cast(stream, 'uint8');
        end
    end   
end