classdef EdgeType
    %EdgeType
    
    properties
        type   = EdgeTypeType.empty;
        pos    = uint8.empty;
        P_dPos = single.empty;
        P_edge = single.empty;
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
            obj.P_dPos(1) = typecast(flip(varargin{1}(3:6)), 'single');
            obj.P_dPos(2) = typecast(flip(varargin{1}(7:10)), 'single');
            obj.P_edge(1) = typecast(flip(varargin{1}(11:14)), 'single');
            obj.P_edge(2) = typecast(flip(varargin{1}(15:18)), 'single');
        end
        
        function stream = pack(obj)
            % pack properties into stream
            stream(1) = obj.type;
            stream(2) = obj.pos;
            stream(3:6) = flip(typecast(obj.P_dPos(1), 'uint8'));
            stream(7:10) = flip(typecast(obj.P_dPos(2), 'uint8'));
            stream(11:14) = flip(typecast(obj.P_edge(1), 'uint8'));
            stream(15:18) = flip(typecast(obj.P_edge(2), 'uint8'));
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