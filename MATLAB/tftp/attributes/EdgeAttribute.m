classdef EdgeAttribute < TftpAttribute
    %EdgeAttribute
 
    methods
        % EdgeAttribute constructor
        function obj = EdgeAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 18; % 1 + 1 + 2*4 + 2*4
            ssize = 1;
            ctype = 'uint8';
            mtype = 'EdgeType';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function edge = decode(stream)
            % cast byte stream
            %stream = typecast(stream, 'uint8');
            
            % initialise object
            edge = EdgeType(stream);
        end
        
        % encode
        function stream = encode(edge)
            % build stream from EdgeType properties
            stream = edge.pack();
            
            % cast to byte stream
            %stream = cast(stream, 'uint8');
        end
    end   
end