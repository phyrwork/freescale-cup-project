classdef TftpRecord
    %TftpRecord 
    %
    
    properties
        attribute = char([]);
        times = single([]);
        size;
        values;
    end
    
    properties (SetAccess = private)
        type = char([]);
    end
    
    % methods
    methods
        % TftpRecord constructor
        function obj = TftpRecord(attribute, varargin)
            % parse arguments
            p = inputParser;
            addRequired(p, 'attribute', @ischar);
            addOptional(p, 'type', 'single', @ischar);
            parse(p, attribute, varargin{:});
            
            % set properties
            obj.attribute = p.Results.attribute;
            obj.type = p.Results.type;
            obj.values = eval([obj.type, '([])']);
        end
        
        % get.size
        % ----------
        % Class preallocates when vectors are full for faster writes
        % so need to call this method to get real length of record
        function size = get.size(obj)
            % times and values operated on in parallel so should be the
            % same as long as nothing else modifies them:
            % if there is a way to set public:read-only that would be
            % useful
            size = find(obj.times ~= 0, 1, 'last');
            if ( isempty(size) )
                size = 0;
            end
        end
        
        % push()
        function obj = push(obj, times, values)
            % validate input
            if ( length(times) ~= length(values) )
                error('Times and values vectors must be the same length');
            end
            
            % extend arrays if necessary
            if ( length(obj.times) < obj.size + length(times) )
                obj.times = [obj.times, zeros(1, 512, 'single')];
                obj.values = [obj.values, zeros(1, 512, obj.type)];
            end
            
            % push values
            start = obj.size + 1;
            finish = obj.size + length(times);
            obj.times(start:finish) = single(times);
            obj.values(start:finish) = cast(values, obj.type);
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

