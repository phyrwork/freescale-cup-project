classdef TftpRecord < handle
    %TftpRecord 
    %
    properties (Constant)
        prealloc_size = 1024;
    end
    
    properties
        attribute = cast([], 'char');
        rsize     = cast(0, 'uint32');
        time;
        times     = cast([], 'single');
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
        function [time, value] = peek(obj)
            % return most recently pushed data
            if (obj.rsize == 0)
                time = [];
                value = [];
            else
                time = obj.times(obj.rsize);
                value = obj.values(:, obj.rsize);
            end
        end
        
        % afterTime
        function [times, values] = latest(obj, period)
            % return values with a time value greater than time
            period = obj.time - period;
            ind = find(obj.times > period, 1, 'first');
            times = obj.times(ind:obj.rsize);
            values = obj.values(:,ind:obj.rsize);
        end
        
        % atTime
        function [time, value] = at(obj, time)
            % return first sample after given time
            ind = find(obj.times > time, 1, 'first');
            time = obj.times(ind);
            value = obj.value(ind);
        end
        
        % between
        function [times, values] = between(obj, start, finish)
            % return samples between given times
            start = find(obj.times > start, 1, 'first');
            if isempty(start)
                start = 1;
            end
            finish = find(obj.times > finish, 1, 'first');
            if isempty(finish)
                finish = obj.rsize;
            end
            times = obj.times(start:finish);
            values = obj.values(start:finish);
        end
    end
    
    % get.methods
    methods
        % get.time
        function time = get.time(obj)
            if (obj.rsize == 0)
                time = 0;
            else
                time = obj.times(obj.rsize);
            end
        end
    end
end

