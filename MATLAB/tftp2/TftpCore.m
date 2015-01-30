classdef TftpCore
    %TftpCore
    
    properties (Constant)
        timerfreq = 10000;
    end
    
    properties (Access = protected)
        modules = {};
    end
    
    methods
        % constructor
        function obj = TftpCore()
            
            % preallocate module array
            obj.modules = cell(1, 2^16);
            
%             obj = obj.addModule(@LinescanAttribute,'01', 'linescan0')
%             obj = obj.addModule(@LinescanAttribute,'02', 'linescan1')
            obj = obj.addModule(@WheelSpeedAttribute,'03', 'w_fl');
            obj = obj.addModule(@WheelSpeedAttribute,'04', 'w_fr');
            obj = obj.addModule(@WheelSpeedAttribute,'05', 'w_rl');
            obj = obj.addModule(@WheelSpeedAttribute,'06', 'w_rr');
%             obj = obj.addModule(@WheelSpeedAttribute,'17', 'w*_rl');
%             obj = obj.addModule(@WheelSpeedAttribute,'18', 'w*_rr');
%             obj = obj.addModule(@MotorDutyAttribute,'13', 'D_rl');
%             obj = obj.addModule(@MotorDutyAttribute,'14', 'D_rr');
%             obj = obj.addModule(@MotorCurrentAttribute,'07', 'i_rl');
%             obj = obj.addModule(@MotorCurrentAttribute,'08', 'i_rr');
%             obj = obj.addModule(@MotorTorqueAttribute,'09', 'T_rl');
%             obj = obj.addModule(@MotorTorqueAttribute,'0A', 'T_rr');
%             obj = obj.addModule(@MotorTorqueAttribute,'11', 'T*_rl');
%             obj = obj.addModule(@MotorTorqueAttribute,'12', 'T*_rr'):
%             obj = obj.addModule(@PidAttribute,'15', 'PID_T_rl');
%             obj = obj.addModule(@PidAttribute,'16', 'PID_T_rr');
%             obj = obj.addModule(@SlipAttribute,'0B', 's_rl');
%             obj = obj.addModule(@SlipAttribute,'0C', 's_rr');
%             obj = obj.addModule(@PositioningStateAttribute,'0D', 'PositioningState');
%             obj = obj.addModule(@LineAttribute,'0E', 'TargetLine');
%             obj = obj.addModule(@TrackPositionAttribute,'0F', 'TrackPosition');
%             obj = obj.addModule(@ProfilerAttribute,'10', 'Profiler');
        end
        
        % parse a frame
        function entries = parse(obj, frame)
            
            % isolate timestamp
            frametime = typecast(frame(1:4), 'uint32') / obj.timerfreq;
            
            % isolate frame start indicies
            i = 5;             % frame read index
            s = [];            % starting positions of frame segments
            l = length(frame); % length of frame
            
            while i+1 < l % continue until end of message has been reached
                
                s = [s,i];                                    % record start index of segment
                i = i + 2 + typecast(frame(i:i+1), 'uint16'); % jump to start of next segment
            end
            
            % form model struct       
            entry.code = [];       
            entry.time = frametime;
            entry.value = [];
            
            % preallocate array of structs
            l = length(s);
            entries = repmat(entry, 1, l); % all entries have timestamp assigned to them already
            
            for f = 1:l % create TftpRecord entry structs from each segment
                
                b = s(f); % base index
                
                vlen = typecast(frame(b:b+1), 'uint16');   % b+(1:0) - segment length
                code = typecast(frame(b+2:b+3), 'uint16'); % b+(3:2) - sample code
                raw  = frame(b+4:b+1+vlen);                % b+(4:end) -  sample value
                
                entries(f).code = code;
                entries(f).value = obj.modules{code}.decode(raw); % decode sample
            end
        end
    end
    
    methods (Access = private)
        % add module
        function obj = addModule(obj, code, module, varargin)
            
            % convert code from hex
            code = hex2dec(code);
            
            % check if code used already
            if ~isempty(obj.modules{code});
                error(['Failed to add module: code ''',dec2hex(code), ''' is already in use.']);
            end
            
            % add module to manifest
            obj.modules{code} = module(varargin{:});
        end
    end
end

