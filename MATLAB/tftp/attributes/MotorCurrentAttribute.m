classdef MotorCurrentAttribute < TftpAttribute
    %AngularVelocityAttribute
 
    methods
        % MotorCurrentAttribute constructor
        function obj = MotorCurrentAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 4;
            ssize = 1;
            ctype = 'single';
            mtype = 'single';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
    end  
end