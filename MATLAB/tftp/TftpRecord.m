classdef TftpRecord
    %TftpRecord 
    %
    properties (Constant)
        prealloc_size = 1024;
    end
    
    properties
        attribute = cast([], 'char');
        times     = cast([], 'single');
        rsize     = cast(0, 'uint32');
        values;
    end
    
    properties (SetAccess = private)
        type = char([]);
    end
    
    % methods
    methods
        % TftpRecord constructor
        function obj = TftpRecord(varargin)
            % parse arguments
            p = inputParser;
            addOptional(p, 'attribute', '[padding_record]',@ischar);
            addOptional(p, 'type', 'single', @ischar);
            addOptional(p, 'ssize', 1, @isnumeric);
            parse(p, varargin{:});
            
            % set properties
            obj.attribute = p.Results.attribute;
            obj.type = p.Results.type;
            obj.times = zeros(1, obj.prealloc_size, 'single');
            obj.values = zeros(p.Results.ssize, obj.prealloc_size, obj.type);
        end
        
        % push()
        function obj = push(obj, time, value)
            % validate input lengths - unnessary now only pushing one value
            % at a time
            %if ( length(time) ~= size(value, 2) )
                %error('Times and values vectors must be the same length');
            %end
            
            % extend arrays if necessary - preallocate for speed
            if ( length(obj.times) == obj.rsize )
                obj.times = [obj.times, zeros(1, obj.prealloc_size, 'single')];
                obj.values = [obj.values, zeros(size(value, 1), obj.prealloc_size, obj.type)];
            end
            
            % push values
            obj.rsize = obj.rsize + 1;
            obj.times(:, obj.rsize) = time;
            obj.values(:, obj.rsize) = value;
        end
        
        % last
        function [time, value] = last(obj)
            % return most recently pushed data
            time = obj.times(obj.rsize);
            value = obj.values(obj.rsize);
        end
        
        % afterTime
        function [times, values] = afterTime(obj, time)
            % return values with a time value greater than time
            ind = find(obj.times > time, 1, 'first');
            times = obj.times(ind:obj.rsize);
            values = obj.values(:,ind:obj.rsize);
        end
        
        % atTime
        function [time, value] = atTime(obj, time)
            % return first sample after given time
            ind = find(obj.times > time, 1, 'first');
            time = obj.times(ind);
            value = obj.value(ind);
        end
    end
    
    % set.methods
    methods
        % set.attribute
        function obj = set.attribute(obj, attribute)
            if ( ~isa(attribute, 'char') )
                error('Attribute name must be of type char()');
            end
            obj.attribute = attribute;
        end
    end
end

