classdef TftpRecord
    %TftpRecord 
    %
    
    properties
        attribute   = cast([], 'char');
        times       = cast([], 'single');
        rsize        = cast([], 'uint32');
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
            obj.times = zeros(1, 512, 'single');
            obj.values = zeros(p.Results.ssize, 512, obj.type);
        end
        
        % get.size
        % ----------
        % Class preallocates when vectors are full for faster writes
        % so need to call this method to get real length of record
        function rsize = get.rsize(obj)
            % times and values operated on in parallel so should be the
            % same as long as nothing else modifies them:
            % if there is a way to set public:read-only that would be
            % useful
            rsize = find(obj.times ~= 0, 1, 'last');
            if ( isempty(rsize) )
                rsize = 0;
            end
        end
        
        % push()
        function obj = push(obj, times, values)
            % validate input lengths
            if ( length(times) ~= size(values, 2) )
                error('Times and values vectors must be the same length');
            end
            
            % extend arrays if necessary
            if ( length(obj.times) < obj.rsize + length(times) )
                obj.times = [obj.times, zeros(1, 512, 'single')];
                obj.values = [obj.values, zeros(size(values, 1), 512, obj.type)];
            end
            
            % push values
            start = obj.rsize + 1;
            finish = obj.rsize + length(times);
            obj.times(:,start:finish) = single(times);
            obj.values(:,start:finish) = cast(values, obj.type);
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

