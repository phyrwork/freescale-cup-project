classdef EdgeTypeType < uint8
    %EdgeTypeType
    
    enumeration
        rising  (0)
        falling (1)
        virtual (255)
    end
   
    methods (Static)
        % zeros support
        function z = zeros(varargin)
            if (nargin == 0)
                % For zeros('CLass')
                z = EdgeTypeType(255);
            elseif any([varargin{:}] <= 0)
                % For zeros with any dimension <= 0
                z = EdgeTypeType.empty(varargin{:});
            else
                % For zeros(m,n,...,'Class')
                % Use property default values
                z = repmat(EdgeTypeType(255),varargin{:});
            end
        end
    end
end

