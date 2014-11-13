classdef Tftp
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
                LinescanAttribute('00', 'linescan0'),...
                LinescanAttribute('01', 'linescan1'),...
                AngularVelocityAttribute('02', 'w_fl'),...
                AngularVelocityAttribute('03', 'w_fr'),...
                AngularVelocityAttribute('04', 'w_rl'),...
                AngularVelocityAttribute('05', 'w_rr')...
                });
        end
    end
    
    % data handling methods (SetAccess = protected) later on...
    methods
        % unpack
        % ------
        % break frame into segments for further handling
        %
        function segments = unpack(obj, frame)
            % extract time code
            time = frame(1:obj.sizeof_time);
            time = obj.formatTime(time);
            f_index = obj.sizeof_time + 1; % advance frame read index
            
            % parse rest of frame into segments
            segments = TftpSegment.empty;
            while ( f_index <= length(frame) )
                % read code and lookup module
                code = frame(f_index);
                m_index = obj.lookupCode(code);
                
                % create segment
                value = frame(f_index + 1:f_index + obj.modules{m_index}.fsize);
                segment = TftpSegment(code, time, value);
                segments = [segments, segment];
                
                % update frame read index
                f_index = f_index + obj.modules{m_index}.fsize + 1;
            end
        end
        
        % pack
        function frame = pack(obj, segments)
            % TODO - this can wait until implementation of commands
        end
        
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
    
    methods (Access = protected)
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
                index = length(obj.codes) + 1;
                obj.codes(index) = module.code;
                obj.modules{index} = module;
            end
        end
        
        % lookup code
        function index = lookupCode(obj, code)
            % 'first' for speed - should only be one match anyways
            index = find(obj.codes == code, 1, 'first');
            if ( isempty(index) )
                index = 0;
                error('Segment code not found.');
            end
        end
        
        % format time
        function time = formatTime(obj, time)
            time = flip(time);
            time = typecast(time, 'uint32');
            time = cast(time, 'single');
        end
    end
end

