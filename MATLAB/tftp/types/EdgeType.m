classdef EdgeType
    %EdgeType
    
    properties
        type   = EdgeTypeType.empty;
        pos    = uint8.empty;
        dpos   = int8.empty;
    end

    methods
        % EdgeType constructor
        function obj = EdgeType(varargin)
            % Check no. args
            if (nargin == 0)
                return;
            end
            
            % Unpack into properties
            obj.type      = varargin{1}(1);
            obj.pos       = varargin{1}(2);
            obj.dpos      = varargin{1}(3);
        end
        
        function stream = pack(obj)
            % pack properties into stream
            stream(1) = obj.type;
            stream(2) = obj.pos;
            stream(3) = obj.dpos;
            stream(3:4) = [0,0]; % padding
        end
    end
    
    methods (Static)
        % zeros support
        function z = zeros(varargin)
            if (nargin == 0)
                % For zeros('CLass')
                z = EdgeType;
            elseif any([varargin{:}] <= 0)
                % For zeros with any dimension <= 0
                z = EdgeType.empty(varargin{:});
            else
                % For zeros(m,n,...,'Class')
                % Use property default values
                z = repmat(EdgeType,varargin{:});
            end
        end
    end
end