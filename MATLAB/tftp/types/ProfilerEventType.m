classdef ProfilerEventType
    %ProfilerEventType
    
    properties
        entry  = uint8.empty;
        event  = ProfilerEventTypeType.empty;
        data   = uint32.empty;
    end

    methods
        % ProfilerEventType constructor
        function obj = ProfilerEventType(varargin)
            % Check no. args
            if (nargin == 0)
                return;
            end
            
            % Unpack into properties
            obj.data  = typecast(varargin{1}(1:4), 'single');
            % (5:6) padding
            obj.event = ProfilerEventTypeType(varargin{1}(7));
            obj.entry = varargin{1}(8);
        end
        
        function stream = pack(obj)
            % pack properties into stream
            stream(1:4) = typecast(obj.data, 'uint8');
            stream(5:6) = [0,0]; % padding
            stream(7)   = cast(obj.event, 'uint8');
            stream(8)   = obj.entry;
        end
    end
    
    methods (Static)
        % zeros support
        function z = zeros(varargin)
            if (nargin == 0)
                % For zeros('CLass')
                z = ProfilerEventType;
            elseif any([varargin{:}] <= 0)
                % For zeros with any dimension <= 0
                z = ProfilerEventType.empty(varargin{:});
            else
                % For zeros(m,n,...,'Class')
                % Use property default values
                z = repmat(ProfilerEventType,varargin{:});
            end
        end
    end
end