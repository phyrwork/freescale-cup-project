classdef TftpLink
    %TftpSerial
    
    properties
        device;
        
    end
    
    methods
        % constructor
        function obj = TftpLink(varargin)
            
            % parse arguments
            p = inputParser;
            addParameter(p, 'id', 'HC-05', @ischar);
            addParameter(p, 'interface', 'Bluetooth', @ischar);
            addParameter(p, 'channel', 1, @isnumeric);
            addParameter(p, 'baud', 480600, @isnumeric);
            
            interface = lower(p.Results.interface);
            
            % establish serial link
            switch interface
                
                % Bluetooth
                case 'bluetooth'
                    obj.device = Bluetooth(p.Results.id, p.Results.channel);
                    
                case 'serial'
                    obj.device = Serial(p.Results.id, p.Results.baud);
                    
                otherwise
                    error(['Interface ', p.Results.interface, ' is not recognised.']);
            end
            
            % setup input buffer and begin reading
            obj.device.InputBufferSize = 2^20; % 1MB buffer
            fopen(obj.device);
        end
        
        % receive
        function frames = receive(obj)
            
             % if no new data, no new frames
            %r = obj.device.BytesAvailable;
            r = 1;
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
            
            % decode and return
            for i = 1:length(frames)
                frames{i} = obj.decode(frames{i}, 512);
            end
        end
        
        % send
        function send(obj)
        end
    end
    
    methods (Static)
        % decode COBS encoded message
        function output = decode( input, mtu )
        
            output = zeros(0, mtu, 'uint8'); % preallocate
            r = 1;

            while( r < length(input) )

                code = input(r);                            % number of bytes to copy
                output(end+1:end+code-1) = input(r+1:r+code-1); % copy bytes

                if( code ~= 255 && r ~= length(input) ) % 255 = 0xFF
                    output(end+1) = 0; % 0 = '\0'
                end

                r = r + code;
            end 
        end
    end
    
end


