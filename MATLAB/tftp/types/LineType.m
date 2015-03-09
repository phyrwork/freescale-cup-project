classdef LineType
    %LineType
    
    properties
        edges = LineType.empty;
        width = uint8.empty;
        P     = struct.empty;
    end

    methods
        % LineType constructor
        function obj = LineType(stream)
            % Check no. args
            if (nargin == 0)
                return;
            end
            
            % Check input type
            obj.edges    = EdgeType(stream(1:3));
            obj.edges(2) = EdgeType(stream(4:6));
            obj.width    = stream(7);
            % 13 - padding
            obj.P(1).asl = typecast(stream(9:12), 'single');
            obj.P(1).rel = typecast(stream(13:16), 'single');
            obj.P(1).fsh = typecast(stream(17:20), 'single');
        end
        
        function stream = pack(obj)
            % pack properties into stream
            stream(1:4)   = obj.edges(1).pack();
            stream(5:8)   = obj.edges(2).pack();
            stream(9)     = typecast(obj.width, 'uint8');
            %10:12 - padding
            stream(13:16) = typecast(obj.P(1).asl, 'uint8');
            stream(17:20) = typecast(obj.P(1).rel, 'uint8');
            stream(21:24) = typecast(obj.P(1).fsh, 'uint8');
        end
    end
    
    methods (Static)
        % zeros support
        function z = zeros(varargin)
            if (nargin == 0)
                % For zeros('CLass')
                z = LineType;
            elseif any([varargin{:}] <= 0)
                % For zeros with any dimension <= 0
                z = LineType.empty(varargin{:});
            else
                % For zeros(m,n,...,'Class')
                % Use property default values
                z = repmat(LineType,varargin{:});
            end
        end
    end
end