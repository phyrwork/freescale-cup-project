classdef Tftp < handle
    %Tftp
    
    properties (Access = private, Constant)
        sizeof_time = 4;
    end
    
    properties (Access = protected)
        codes   = cast([], 'uint8');
        modules = {};
    end
    
    % constructor: protocol definition
    methods
        function obj = Tftp()
            % list of attached modules (attributes/commands)
            obj = obj.addModules({...
                LinescanAttribute('01', 'linescan0'),...
                LinescanAttribute('02', 'linescan1'),...
                AngularVelocityAttribute('03', 'w_fl'),...
                AngularVelocityAttribute('04', 'w_fr'),...
                AngularVelocityAttribute('05', 'w_rl'),...
                AngularVelocityAttribute('06', 'w_rr'),...
                MotorCurrentAttribute('07', 'i_rl'),...
                MotorCurrentAttribute('08', 'i_rr'),...
                MotorTorqueAttribute('09', 'T_rl'),...
                MotorTorqueAttribute('0A', 'T_rr'),...
                SlipAttribute('0B', 's_rl'),...
                SlipAttribute('0C', 's_rr'),...
                PositioningStateAttribute('0D', 'PositioningState'),...
                LineAttribute('0E', 'TargetLine'),...
                TrackPositionAttribute('0F', 'TrackPosition');
            });
        end
    end
    
    % data handling methods (SetAccess = protected) later on...
    methods
        % parse
        % ------
        % break frame into segments for further handling
        %
        function segments = parse(obj, frame)
            % extract time code
            time = frame(1:obj.sizeof_time);
            % time = obj.formatTime(time);
            time = flip(time);
            time = typecast(time, 'single');
            
            index = obj.sizeof_time + 1; % advance frame read index
            
            % parse rest of frame into segments
            s = 0;
            while ( index <= length(frame) )
                % parse code and get module parameters
                code = frame(index);
                %fsize = obj.modules{code}.fsize;
                
                % parse value and decode
                value = frame(index + 1:index + obj.modules{code}.fsize);
                value = obj.modules{code}.decode(value);
                
                % create segment
                s = s + 1;
                segment.code = code;
                segment.time = time;
                segment.value = value;
                segments(s) = segment;
                %segments(s) = TftpSegment(code, time, value);
                
                % update frame read index
                index = index + obj.modules{code}.fsize + 1;
            end
        end
        
        % pack
        %function frame = pack(obj, segments)
            % TODO - this can wait until implementation of commands
        %end
        
        % getAttributeType
        function type = getAttributeType(obj, attribute)
            for i = 1:length(obj.modules)
                if ( strcmp(obj.modules{i}.attribute, attribute) )
                    type = obj.modules{i}.mtype;
                    return;
                end
            end
            error(['No match for attribute ''',attribute,''' found in loaded modules.']);
        end
    end
    
    methods (Access = private)
        % add modules
        function obj = addModules(obj, modules)
            for i = 1:length(modules)
                % for each module
                module = modules{i};
                
                % check if code used already
                f = find(obj.codes == module.code, 1);
                if ( ~isempty(f) )
                    error(['Failed to add module: code ''',dec2hex(module.code), ''' is already in use.']);
                end

                % add module
                obj.modules{module.code} = module;
            end
        end
    end
end

