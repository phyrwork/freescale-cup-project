classdef LinescanAttribute < TftpAttribute
    %LinescanAttribute 
 
    methods
        % LinescanAttribute constructor
        function obj = LinescanAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 256;
            ssize = 128;
            ctype = 'uint16';
            mtype = 'single';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
        
        % decode
        function value = decode(obj, value)
            % cast byte stream
            value = decode@TftpAttribute(obj, value);
            
            % normalise
            value = value;
            value = transpose(value); % rotate to fit in TftpRecord properly
        end
        
        % encode
        function value = encode(obj, value)
            % denormalise
            value = transpose(value); % derotate
            value = value;
            
            % cast to byte stream
            value = encode@TftpAttribute(obj, value);
        end
    end   
end