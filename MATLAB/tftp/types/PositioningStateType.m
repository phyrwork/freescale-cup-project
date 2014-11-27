classdef PositioningStateType < uint8
    %PositioningStateType
    
    enumeration
        complete  (0)
        partial_left (10)
        partial_right (20)
        none (255)
    end
    
    methods (Static)
        % zeros support
        function z = zeros(varargin)
            if (nargin == 0)
                % For zeros('CLass')
                z = PositioningStateType(255);
            elseif any([varargin{:}] <= 0)
                % For zeros with any dimension <= 0
                z = PositioningStateType.empty(varargin{:});
            else
                % For zeros(m,n,...,'Class')
                % Use property default values
                z = repmat(PositioningStateType(255),varargin{:});
            end
        end
    end
end

