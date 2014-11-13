classdef AngularVelocityAttribute < TftpAttribute
    %AngularVelocityAttribute 
    
    properties (Constant)

    end
 
    methods
        % LinescanAttribute constructor
        function obj = AngularVelocityAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 4;
            ctype = 'single';
            mtype = 'single';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ctype, mtype);      
        end
    end  
end