classdef TrackPositionAttribute < TftpAttribute
    %AngularVelocityAttribute 
 
    methods
        % TrackPositionAttribute constructor
        function obj = TrackPositionAttribute(code, attribute)
            % set default properties used in superclass constructor
            fsize = 1;
            ssize = 1;
            ctype = 'int8';
            mtype = 'int8';
            
            % call TftpAttribute constructor
            obj = obj@TftpAttribute(code, attribute, fsize, ssize, ctype, mtype);      
        end
    end  
end