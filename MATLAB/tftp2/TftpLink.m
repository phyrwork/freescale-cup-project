classdef TftpLink
    %TftpSerial
    
    properties
        device;
        
    end
    
    methods
        % constructor
        function obj = TftpLink(id, varargin)
            
        end
        
        % receive
        function frames = receive(obj)
            
             % if no new data, no new frames
            r = obj.device.BytesAvailable;
            if (r == 0)
                return;
            end
            
            % read data
            [b, ~] = fread(obj.device, r);
            
            % split into frames
            f = [0; find(b == 0)];          % find boundaries
            frames = cell(1, length(f)-1);  % preallocate cell header
            for i = 3:length(f)
            frames{i-1} = b(f(i-1)+1:f(i)); % slice between byte after last boundary and next boundary
            end                             % data in d{i} is vertical
            
            % deal with leftovers
            persistent l;
            frames{1} = [l;b(1:f(2))]; %  piece together last part of previous message with first part of this one
            
            if length(b) > f(end) % if message continues past last boundary
                l = b(f(end)+1:end); % store the last part of this message
            else
                l = []; % otherwise clear the leftovers buffer
            end
        end
        
        % send
        function send(obj)
        end
    end
    
end

