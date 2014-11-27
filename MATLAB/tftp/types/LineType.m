classdef LineType
    %LineType
    
    properties
        edges    = LineType.empty;
        width    = uint8.empty;
        P_width  = single.empty;
        P_dWidth = single.empty;
        P_line   = single.empty;
    end

    methods
        % LineType constructor
        function obj = LineType(stream)
            % Check no. args
            if (nargin == 0)
                return;
            end
            
            % Check input type
            obj.edges    = EdgeType(stream(1:20));
            obj.edges(2) = EdgeType(stream(21:40));
            obj.width    = stream(41);
            % (42:44) padding
            obj.P_width  = typecast(flip(stream(45:48)), 'single');
            obj.P_dWidth = typecast(flip(stream(49:52)), 'single');
            obj.P_line   = typecast(flip(stream(53:56)), 'single');
        end
        
        function stream = pack(obj)
            % pack properties into stream
            stream(1:20)  = obj.edges(1).pack();
            stream(21:40) = obj.edges(2).pack();
            stream(41)    = obj.width;
            stream(42:44) = [0,0,0]; % padding
            stream(45:48)  = flip(typecast(obj.P_width, 'uint8'));
            stream(49:52) = flip(typecast(obj.P_dWidth, 'uint8'));
            stream(53:56) = flip(typecast(obj.P_line, 'uint8'));
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