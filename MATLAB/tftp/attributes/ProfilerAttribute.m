classdef ProfilerAttribute < TftpAttribute
    %ProfilerAttribute
 
    methods
        % ProfilerAttribute constructor
        function obj = ProfilerAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 8;
            ssize = 1;
            ctype = 'uint8';
            mtype = 'ProfilerEventType';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function event = decode(obj, stream)
            % cast byte stream
            %stream = typecast(stream, 'uint8');
            
            % initialise object
            event = ProfilerEventType(stream);
        end
        
        % encode
        function stream = encode(obj, event)
            % build stream from EdgeType properties
            stream = event.pack();
            
            % cast to byte stream
            %stream = cast(stream, 'uint8');
        end
    end   
end